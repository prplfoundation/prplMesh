#!/bin/bash 

# In order to use the script in non interactive mode, 
# enter first argument to be the repo name, one of the following: 'framework', 'common', 'controller', 'agent'

#set -x

echo kwcheck --version
echo current folder: "$(pwd)"
echo number of input arguments: "$#"

declare -a REPOS=("framework" "common" "controller" "agent")
declare TOOLCHAIN_PATH
declare URL_PATH
declare REPORT_PATH
declare REPO_PATH
declare ROOT_PATH
declare PLATFORM_TYPE

# set an initial value for the flag
PASSIVE_MODE=false

# read the options
OPTS=$(getopt -o p -n 'kw.sh' -- "$@")
eval set -- "$OPTS"

# extract options and their arguments into variables.
while true ; do
      case "$1" in
            "") break ;;
            -p) PASSIVE_MODE=true; shift; break ;;
            * ) break ;;
      esac   
done

################################################################
####################### Local Functions ########################
################################################################

prepare_kw()
{
      ROOT_PATH=$(realpath "$(pwd)"/../../)
      REPO_PATH=$(realpath "$(pwd)"/../../"$REPO")
      declare TREE_PATH # only needed in this function
      if [ -f ./../../external_toolchain.cfg ]; then
            PLATFORM_TYPE=$(grep -Po "(?<=^TARGET_PLATFORM=).*" "$(realpath "$(pwd)"/../../external_toolchain.cfg)") # "ugw"/"rdkb"
            TREE_PATH=$(grep -Po "(?<=^PLATFORM_BASE_DIR=).*" "$(realpath "$(pwd)"/../../external_toolchain.cfg)")
            echo Found tree path: "$TREE_PATH"
      else 
            echo Did no found 'external_toolchain.cfg', assuming Linux enviorment
            PLATFORM_TYPE="linux"
      fi
      echo platfrom identified: $PLATFORM_TYPE
      REPORT_PATH=$REPO_PATH/kw_reports/$PLATFORM_TYPE
      mkdir -p "$REPORT_PATH"

      # Remove old compilation script
      rm -rf _GO_KW

      # Generate input script to klocwork checker
      if [ "$PLATFORM_TYPE" = "rdkb" ]; then
            TOOLCHAIN_PATH=$(realpath "$(pwd)"/../../../../atom_rdkbos/build/tmp/work/core2-32-rdk-linux)
            URL_PATH="https://klocwork3-jf.devtools.intel.com:8140/Atom-Puma7-RDKB"

            # Generate compilation script for RDKB tree enviorment to be used by klocwork checker
            {
                  echo -e "#!/bin/bash"
                  echo -e "cd ../../../../setup/"
                  echo -e "source puma_setup -o"
                  echo -e "echo starting kw from folder: \`pwd\`"
                  echo -e "echo bitbake -c cleanall multiap-$REPO"
                  echo -e "bitbake -c cleanall multiap-$REPO"
                  echo -e "echo bitbake -c compile multiap-$REPO"
                  echo -e "bitbake -c compile multiap-$REPO"
                  echo -e "exit"
            } > _GO_KW

      elif [ "$PLATFORM_TYPE" = "ugw" ]; then
            TOOLCHAIN_PATH=$(grep -Po "(?<=^PLATFORM_BASE_DIR=).*" "$(realpath "$(pwd)"/../../external_toolchain.cfg)")
            URL_PATH="https://klocwork-iind4.devtools.intel.com:8105/UGW_master_grx350_rt"

            # Generate compilation script for UGW tree enviorment to be used by klocwork checker
            {
                  echo -e "#!/bin/bash"
                  echo -e "cd $TREE_PATH"
                  echo -e "echo starting kw from folder: \`pwd\`"
                  echo -e "echo make package/feeds/ugw_packages/multiap_$REPO/{clean,compile,install}"
                  echo -e "make package/feeds/ugw_packages/multiap_$REPO/{clean,compile,install}"
                  echo -e "exit"
            } > _GO_KW

            # Set compilation on UGW tree to use multiap standalone folders
            sed -i -e "s|# CONFIG_multiap_${REPO}_USE_CUSTOM_SOURCE_DIR is not set|CONFIG_multiap_${REPO}_USE_CUSTOM_SOURCE_DIR=y|g" "$TREE_PATH"/.config
            if ! grep -q "CONFIG_multiap_${REPO}_CUSTOM_SOURCE_DIR" "$TREE_PATH"/.config; then
                  sed -i -e "/CONFIG_multiap_${REPO}_USE_CUSTOM_SOURCE_DIR/aCONFIG_multiap_${REPO}_CUSTOM_SOURCE_DIR=\"${REPO_PATH}\"" "$TREE_PATH"/.config
            fi
            

      else # linux/other

            PASSIVE_MODE_OPT=""
            if $PASSIVE_MODE ; then
                  PASSIVE_MODE_OPT="-f PASSIVE_MODE=ON"
                  echo "PASSIVE_MODE=ON"
            else
                  echo "PASSIVE_MODE=OFF"
            fi

            # Generate compilation script using maptools script to be used by klocwork checker
            {
                  echo -e "#!/bin/bash"
                  echo -e "cd ../"
                  echo -e "echo starting kw from folder: \`pwd\`"
                  echo -e "echo ./maptools.py build $REPO $PASSIVE_MODE_OPT -c clean make"
                  echo -e "./maptools.py build $REPO $PASSIVE_MODE_OPT -c clean make"
                  echo -e "exit"
            } > _GO_KW

      fi

      chmod +x _GO_KW
}

kw()
{ 
      echo Performing KW on: "$REPO".

      #prepare any build specific and paths before common section
      prepare_kw

      # Create a klocwork project based on the feeds compilation
      rm -rf .kw*

      echo Connecting to remote klocwork server...
      kwcheck create --url $URL_PATH || \
      { 
            echo "*** ERROR: Connecting to KW server has failed *** ";
            echo making sure klocwork is authenticated to the correct server, and try connect again
            if [ "$PLATFORM_TYPE" = "rdkb" ]; then
                  kwauth --url https://klocwork3-jf.devtools.intel.com:8140 || \
                  {
                        echo "*** ERROR: retval=$?, Could not authenticated to Klocwork server. exit. *** "
                        exit
                  }
                  echo reconnecting to remote klocwork server...
                  kwcheck create --url $URL_PATH || \
                  {
                        echo "*** ERROR: retval=$?, Could not reconnect to Klocwork server. exit. *** "
                        exit
                  }
            elif [ "$PLATFORM_TYPE" = "ugw" ]; then
                  kwauth --url https://klocwork-iind4.devtools.intel.com:8105
                  {
                        echo "*** ERROR: retval=$?, Could not authenticated to Klocwork server. exit. *** "
                        exit
                  }
                  echo reconnecting to remote klocwork server...
                  kwcheck create --url $URL_PATH || \
                  {
                        echo "*** ERROR: retval=$?, Could not reconnect to Klocwork server. exit. *** "
                        exit
                  }
            else 
                  echo "Creating local klockwork server"
                  kwcheck create
            fi 
      }

      echo starting klockwork process
      kwshell -s ./_GO_KW

      if [ "$PLATFORM_TYPE" = "ugw" ]; then
      # Add checkers/overrides that are used by UGW for SDL
      git archive --remote=ssh://git@gts-chd.intel.com:29418/sw_ugw/ugw_sw.git HEAD:kw_support/ kw_override.h | tar -x
      git archive --remote=ssh://git@gts-chd.intel.com:29418/sw_ugw/ugw_sw.git HEAD:kw_support/ klocwork_database.kb | tar -x
      git archive --remote=ssh://git@gts-chd.intel.com:29418/sw_ugw/ugw_sw.git HEAD:kw_support/ analysis_profile.pconf | tar -x
      kwcheck import kw_override.h
      kwcheck import klocwork_database.kb
      kwcheck import analysis_profile.pconf
      fi

      # Analyze and generate reports
      kwcheck run -j auto
      echo ""
      echo Generating reports...
      kwcheck list -F detailed --status 'Analyze','Fix' --report "${REPORT_PATH}"/kwreport_all.log
      kwcheck list -F detailed --severity 1 --status 'Analyze','Fix' --report "${REPORT_PATH}"/kwreport_critical.log
      kwcheck list -F detailed --severity 2 --status 'Analyze','Fix' --report "${REPORT_PATH}"/kwreport_error.log
      kwcheck list -F detailed --severity 3 --status 'Analyze','Fix' --report "${REPORT_PATH}"/kwreport_warning.log
      kwcheck list -F detailed --severity 4 --status 'Analyze','Fix' --report "${REPORT_PATH}"/kwreport_review.log
      kwcheck list -F detailed --status 'Ignore','Not a Problem','Defer' --report "${REPORT_PATH}"/kwreport_ignore.log

      # finalize reports
      # remove local prefixes from source controlled reports
      declare -a KW_REPORTS=(${REPORT_PATH}/kwreport_all.log ${REPORT_PATH}/kwreport_critical.log ${REPORT_PATH}/kwreport_error.log ${REPORT_PATH}/kwreport_warning.log ${REPORT_PATH}/kwreport_review.log ${REPORT_PATH}/kwreport_ignore.log)
      for r in "${KW_REPORTS[@]}"; do
            cp "$r" "${r}".tmp
            sed -i -e "s/${ROOT_PATH////\\/}\///g" "$r" # remove local path prefixes from multiap modules
            sed -i -e "s/${TOOLCHAIN_PATH////\\/}\///g" "$r" # remove local path prefixes from external toolchain files
      done

      # Generate output summary
      declare -a KW_TYPES=("1:Critical" "2:Error" "3:Warning" "4:Review")
      echo -e "Summary by components:" > "${REPORT_PATH}"/kwreport_summary.log
      cp "${REPORT_PATH}"/kwreport_all.log "${REPORT_PATH}"/kwreport_tmp.log
      for t in "${KW_TYPES[@]}"; do
            issue_cnt=$(grep -i "$t" "${REPORT_PATH}"/kwreport_all.log | grep -civ "Ignore\|Not a Problem\|Defer")
            echo "    $t: $issue_cnt" >> "${REPORT_PATH}"/kwreport_summary.log
      done

      rm "${REPORT_PATH}"/kwreport_tmp.log
      echo -e "\nLast KW: $(date +'%d/%m/%Y %H:%M')" >> "${REPORT_PATH}"/kwreport_summary.log

      echo ""
}

################################################################
####################### Script begining ########################
################################################################

# Repo Select
read -pr "On which repo do you with to perfrom klocwork? [0-all, 1-framework, 2-common, 3-controller, 4-agent]: " REPO
case $REPO in
      "0") REPO="all"          ;;
      "1") REPO="framework"    ;;
      "2") REPO="common"       ;;
      "3") REPO="controller"   ;;
      "4") REPO="agent"        ;;
      *)   
            echo "Error: unrecognized input value:'$REPO'" 
            exit 128 # Invalid argument to exit
            ;;
esac

if [ "$REPO" = "all" ]; then
      echo "Performing KW on all repos!" 
      for REPO in "${REPOS[@]}"; do
            kw
      done
else
      kw
fi

echo DONE!

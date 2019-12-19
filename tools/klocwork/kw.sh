#!/bin/bash 

# In order to use the script in non interactive mode, 
# enter first argument to be the repo name, one of the following: 'framework', 'common', 'controller', 'agent'

#set -x

echo kwcheck --version
echo current folder: "$(pwd)"
echo number of input arguments: "$#"

declare -a REPOS=("framework" "common" "controller" "agent")
declare URL_PATH
declare REPORT_PATH
declare REPO_PATH
declare ROOT_PATH
declare PLATFORM_TYPE

#prepare any build specific and paths before common section

ROOT_PATH=$(realpath "$(pwd)"/../../)
PLATFORM_TYPE=$PRPLMESH_PLATFORM_TYPE

echo platfrom identified: $PLATFORM_TYPE

# Remove old compilation script
rm -rf _GO_KW

# Generate input script to klocwork checker
if [ "$PLATFORM_TYPE" = "rdkb" ]; then
echo "Platform type is RDKB"
URL_PATH="https://klocwork3-jf.devtools.intel.com:8140/Atom-Puma7-RDKB"

# Generate compilation script for RDKB tree enviorment to be used by klocwork checker
{
      echo -e "#!/bin/bash"
      echo -e "cd .." #to /tools/
      echo -e "echo starting kw from folder: \`pwd\`"
      echo -e "echo cleaning prplMesh build: ./maptools.sh build map -f -c clean"
      echo -e "./maptools.sh build map -f -c clean"
      echo -e "echo building prplMesh: ./maptools.sh build map -f MSGLIB=None CMAKE_BUILD_TYPE=DEBUG"
      echo -e "./maptools.sh build map -f MSGLIB=None CMAKE_BUILD_TYPE=DEBUG"
      echo -e "exit"
} > _GO_KW

elif [ "$PLATFORM_TYPE" = "ugw" ]; then
echo "Platform type is UGW"
URL_PATH="https://klocwork-iind4.devtools.intel.com:8105/UGW_master_grx350_rt"

# Generate compilation script for UGW tree enviorment to be used by klocwork checker
{
      echo -e "#!/bin/bash"
      echo -e "cd .." #to /tools/
      echo -e "echo starting kw from folder: \`pwd\`"
      echo -e "echo cleaning prplMesh build: ./maptools.sh build map -f -c clean"
      echo -e "./maptools.sh build map -f -c clean"
      echo -e "echo building prplMesh: ./maptools.sh build map -f MSGLIB=None CMAKE_BUILD_TYPE=DEBUG"
      echo -e "./maptools.sh build map -f MSGLIB=None CMAKE_BUILD_TYPE=DEBUG"
      echo -e "exit"
} > _GO_KW


else # ubuntu

# Generate compilation script using maptools script to be used by klocwork checker
{
      echo -e "#!/bin/bash"
      echo -e "cd .." #to /tools/
      echo -e "echo starting kw from folder: \`pwd\`"
      echo -e "echo cleaning prplMesh build: ./maptools.sh build map -f -c clean"
      echo -e "./maptools.sh build map -f -c clean"
      echo -e "echo building prplMesh: ./maptools.sh build map -f MSGLIB=zmq CMAKE_BUILD_TYPE=DEBUG"
      echo -e "./maptools.sh build map -f MSGLIB=zmq CMAKE_BUILD_TYPE=DEBUG"
      echo -e "exit"
} > _GO_KW

fi

chmod +x _GO_KW

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
for REPO in "${REPOS[@]}"; do
      REPO_PATH=$(realpath "$(pwd)"/../../"$REPO")
      REPORT_PATH=$REPO_PATH/kw_reports/$PLATFORM_TYPE
      mkdir -p "$REPORT_PATH"

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
            # TODO: fix path joining problem (capitalization, splitting with slashes)
            sed -i -e "s/${ROOT_PATH////\\/}\///g" "$r" # remove local path prefixes from multiap modules
            sed -i -e "s/${TOOLCHAIN_PATH////\\/}\///g" "$r" # remove local path prefixes from external toolchain files
            rm "${r}".tmp
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
     
done
echo ""
echo DONE!

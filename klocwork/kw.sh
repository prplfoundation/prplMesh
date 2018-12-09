#!/bin/bash 

# In order to use the script in non interactive mode, 
# enter first argument to be the repo name, one of the following: 'framework', 'common', 'controller', 'agent'

#set -x

echo kwcheck --version
echo current folder: `pwd`
echo number of input arguments: "$#"

declare -a REPOS=("framework" "common" "controller" "agent")

################################################################
####################### Local Functions ########################
################################################################

kw()
{ 
echo Performing KW on: $REPO.

# Generate input script to klocwork checker
rm -rf _GO_KW
cat  > _GO_KW << DONE
#!/bin/sh
cd `pwd`/../../$REPO
echo "starting kw from folder: \`pwd\`"
../tools/maptools.py build $REPO -c clean make
exit
DONE

# Create a klocwork project based on the feeds compilation
rm -rf .kw*/
kwcheck create
chmod +x _GO_KW
kwshell -s ./_GO_KW

# Add checkers/overrides that are used by UGW for SDL
git archive --remote=ssh://git@gts-chd.intel.com:29418/sw_ugw/ugw_sw.git HEAD:kw_support/ kw_override.h | tar -x
git archive --remote=ssh://git@gts-chd.intel.com:29418/sw_ugw/ugw_sw.git HEAD:kw_support/ klocwork_database.kb | tar -x
git archive --remote=ssh://git@gts-chd.intel.com:29418/sw_ugw/ugw_sw.git HEAD:kw_support/ analysis_profile.pconf | tar -x
kwcheck import kw_override.h
kwcheck import klocwork_database.kb
kwcheck import analysis_profile.pconf

# Analyze and generate reports
REPORT_PATH=`pwd`"/../../$REPO/kw_reports"
mkdir -p $REPORT_PATH
kwcheck run
echo ""
echo Generating reports...
kwcheck list -F detailed --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_all.log
kwcheck list -F detailed --severity 1 --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_critical.log
kwcheck list -F detailed --severity 2 --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_error.log
kwcheck list -F detailed --severity 3 --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_warning.log
kwcheck list -F detailed --severity 4 --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_review.log

# Generate output summary
declare -a KW_TYPES=("1:Critical" "2:Error" "3:Warning" "4:Review")

echo -e "Summary by components:" > ${REPORT_PATH}/kwreport_summary.log
cp ${REPORT_PATH}/kwreport_all.log ${REPORT_PATH}/kwreport_tmp.log
for t in ${KW_TYPES[@]}; do
      issue_cnt=`grep -c $t ${REPORT_PATH}/kwreport_all.log`
      echo "    $t: $issue_cnt" >> ${REPORT_PATH}/kwreport_summary.log
done
rm ${REPORT_PATH}/kwreport_tmp.log
echo -e "\nLast KW: `date +'%d/%m/%Y %H:%M'`" >> ${REPORT_PATH}/kwreport_summary.log

echo ""

}

################################################################
####################### Script begining ########################
################################################################

# Repo Select
read -p "On which repo do you with to perfrom klocwork? [1-framework, 2-common, 3-controller, 4-agent, 5-all]: " REPO
case $REPO in
      "1") REPO="framework"    ;;
      "2") REPO="common"       ;;
      "3") REPO="controller"   ;;
      "4") REPO="agent"        ;;
      "5") REPO="all"          ;;
      *)   
            echo "Error: unrecognized input value:'$REPO'" 
            exit 128 # Invalid argument to exit
            ;;
esac

if [ "$REPO" == "all" ]; then
      echo "Performing KW on all repos! \n" 
      for REPO in ${REPOS[@]}; do
            kw
      done
else
      kw
fi

echo DONE!




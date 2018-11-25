#!/bin/bash 

#set -x

echo kwcheck --version
echo "current folder: "`pwd`

# Repo Select
read -p "On which repo do you with to perfrom klocwork? [1-framework, 2-common, 3-controller, 4-agent]: " REPO
case $REPO in
      "1") REPO="framework"    ;;
      "2") REPO="common"       ;;
      "3") REPO="controller"   ;;
      "4") REPO="agent"        ;;
      *)   
            echo "Error: unrecognized input value:'$REPO'" 
            exit 128 # Invalid argument to exit
            ;;
esac

# Report Severity Select
read -p "Enter severity seperated [1-Critical & Error (default), 2-Warning & Review, 3-All]: " SEVERITY
case $SEVERITY in
      "1")  SEVERITY="1,2"     ;;
      "2")  SEVERITY="3,4"     ;;
      "3")  SEVERITY="1,2,3,4" ;;
      "")   SEVERITY="1,2"     ;;
      *)   
            echo "Error: unrecognized input value:'$SEVERITY'" 
            exit 128 # Invalid argument to exit
            ;;
esac 

# Clean Repos
CLEAN_PATH=`pwd`"/../../$REPO/build"
echo "cleaning $CLEAN_PATH"
rm -rf $CLEAN_PATH/*

echo "Performing KW on: $REPO"

# Generate input script to klocwork checker
rm -rf _GO_KW
cat  > _GO_KW << DONE
#!/bin/sh
echo "starting kw from folder: `pwd`"
cd ../../$REPO
export STAGING_DIR=""
cmake -H. -B./build -DCMAKE_TOOLCHAIN_FILE=external_toolchain.cmake -DCMAKE_INSTALL_PREFIX:FILEPATH=../out && cmake --build ./build -- install -j
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
kwcheck list -F detailed --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_detailed.log
kwcheck list -F detailed --severity $SEVERITY --status 'Analyze','Fix' --report ${REPORT_PATH}/kwreport_high.log



open a new shell and make sure you are using the new path:
>> which kwauth
Answer should be: /opt/ugw/klockwork/kw123/bin/kwauth

Authenticate user with Klockwork server:
>> kwauth --url https://plxv7007.pdx.intel.com:8140/
      
Login: [WIN_USER_NAME]
Password: [WIN_PASSWORD]

# If the previous direction did not work, the klocwork is not intalled.
# In that case, follow the "Klocwork desktop - standalone configuration (unconnected to KW Server)" direction on:
https://wiki.ith.intel.com/display/CHDSE/Klocwork+on+Linux+Build+Servers+How-To
# and replace the installation path to: "/opt/ugw/klockwork/kw123/"
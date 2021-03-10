$siteName="dada2“ #与FTP用户名一样
$ftpusername = $siteName
$access = "Read,Write"


#添加FTP发布
New-WebBinding -Name $siteName -Port 21 -Protocol ftp -IPAddress * 

#设置FTP属性
Set-WebConfigurationProperty \
-pspath 'MACHINE/WEBROOT/APPHOST'  \
-filter "system.applicationHost/sites/site[@name='$siteName']/ftpServer/security/ssl" \
-name "controlChannelPolicy" \
-value "SslAllow"

Set-WebConfigurationProperty \
-pspath 'MACHINE/WEBROOT/APPHOST'  \
-filter "system.applicationHost/sites/site[@name='$siteName']/ftpServer/security/ssl" \
-name "dataChannelPolicy" \
-value "SslAllow"

Set-WebConfigurationProperty \
-pspath 'MACHINE/WEBROOT/APPHOST'  \
-filter "system.applicationHost/sites/site[@name='$siteName']/ftpServer/security/authentication/anonymousAuthentication" \
-name "enabled" \
-value "true"

Set-WebConfigurationProperty \
-pspath 'MACHINE/WEBROOT/APPHOST'  \
-filter "system.applicationHost/sites/site[@name='$siteName']/ftpServer/security/authentication/basicAuthentication" \
-name "enabled" \
-value "true"

#分配账户
Add-WebConfigurationProperty -pspath 'MACHINE/WEBROOT/APPHOST' -location "$siteName" -filter "system.ftpServer/security/authorization" -name "." -value @{accessType='Allow';roles="$ftpusername";permissions="$access"}


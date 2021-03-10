param(
   $name,
   $PhysicalPath,
   $BindingInformation,
   $protocol

)

New-IISSite -Name $name -PhysicalPath $PhysicalPath -BindingInformation $BindingInformation -Protocol $protocol 


#Bindings                   : {http *:80:, http *:80:1000.red}
#BindingInformation         '*:80:'
#protocol                    http
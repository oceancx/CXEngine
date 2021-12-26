fname=$1
hfile=$fname".h"
cppfile=$fname".cpp"
rm $hfile $cppfile
touch $hfile $cppfile
echo "/*author by: oceancx*/
#pragma once
#include \"defines.h\"

class $fname
{
public:
	$fname();
	~$fname();
};" >> $hfile
echo "/*author by: oceancx*/
#include \"$hfile\"
$fname::$fname()
{

}

$fname::~$fname()
{
	
}" >> $cppfile
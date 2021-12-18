#include "window.h"
#include <script_system.h>
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "file_system.h"
#include "cxlua.h"

#include "math/vector.h"

using namespace cx;

int main(int argc, char *argv[])
{
	handle_command_args(argc,argv);
	FileSystem::InitWorkPath();

	script_system_prepare_init();
	script_system_run_main_script();
	 
	/*Vector3 v1;
	Vector3 v2;
	v1.x = 3;
	v1.y = 4;
	v1.z = 5;
	v1.Print();
	v1[1] = 10;
	v1.Print();

	v2.x = 1;
	v2.y = 1;
	v2.z = 1;
	v1 += v2;

	printf("x %f y %f z %f\n", v1.x, v1.y, v1.z);

	cx::Quaternion lhs;
	cx::Quaternion rhs;
	bool check = lhs == rhs;*/

	// Vector3::up.Print();

	return 0;
}

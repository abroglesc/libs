#ifndef __TEST_FILLERS_DEFS_H
#define __TEST_FILLERS_DEFS_H
#include <unistd.h>

#include <sys/syscall.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "test_fillers.h"

// increment/decrement this when adding/removing a test
#define TEST_FILLER_MAX_DEFS = 1;

TEST_FILLER(
	renameat2_example,
	{
		int src_fd = open("/tmp", __O_PATH);
		int dest_fd = open("/tmp", __O_PATH);
		const char *src_path = "falco_test_filethatdoesnotexists";
		const char *dest_path = "falco_test_filethatdoesnotexists_new";

		unsigned int flags = RENAME_NOREPLACE;
		syscall(SYS_renameat2, src_fd, src_path, dest_fd, dest_path, flags);
	},
	{
		GUARD_SYSCALL_EXIT(renameat2)

		uint16_t *lens = (uint16_t *)((char *)evt + sizeof(struct ppm_evt_hdr));
		char *valptr = (char *)lens + evt->nparams * sizeof(uint16_t);
		ASSERT_TRUE(evt->nparams == 6)
		for(int j = 0; j < evt->nparams; ++j)
		{
			const struct ppm_param_info *param_info = &(info->params[j]);

			switch(param_info->type)
			{
			case PT_CHARBUF:
			{
				fprintf(stdout, " %s", valptr);
			}
			case PT_ERRNO:
			{
				int64_t val = *(int64_t *)valptr;
				ASSERT_TRUE(val == -1);
			}
			default:
			{
				fprintf(stdout, " %s", valptr);
			};
			}

			fprintf(stdout, "\n");
			valptr += lens[j];
		}

		return LIBBPF_PERF_EVENT_DONE;
	})

#endif //__TEST_FILLERS_DEFS_H
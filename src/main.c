#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define VERSION "1.0.0#dev"


int main(int argc, char *argv[]) {
	int index_options = 0;
	int coption = 0;
	char *options_string = "c:vh";
	char *conf = {0};

	static struct option long_options[] = {
		{"config", required_argument, NULL, '-'},
		{"version", no_argument, NULL, '-'},
		{"help", no_argument, NULL, '-'}
	};

	while(1) {
		coption = getopt_long(argc, argv, options_string, long_options, &index_options);

		if(coption)
			break;

		switch(coption) {
			case '-':
				switch(index_options) {
					case 0:
						if(optarg) {
							conf = strdup(optarg);
							fprintf(stderr, "--config %s\n", conf);
						} else {
							fprintf(stderr, "--config no param\n");
							return -1;
						}
					case 1:
						fprintf(stderr, "version : %s\n", VERSION);
						return 0;
					case 2:
						fprintf(stderr, "Usage : %s[c:vh]\n"
								"--config , -c \n config file name\n"
								"--version , -v \n version number\n"
								"--help , -h \n print help information\n", argv[0]);
						return 0;
					default:
						fprintf(stderr, "options error !\n");
						break;
				}
			case 'c':
				if(optarg) {
					conf = strdup(optarg);
					fprintf(stderr, "-c %s\n", conf);
				} else {
					fprintf(stderr, "-c no param\n");
					return -1;
				}
				break;
			case 'v':
				fprintf(stderr, "version : %s\n", VERSION);
				return 0;
			case 'h':
				fprintf(stderr, "Usage : %s[c:vh]\n"
						"--config , -c \n config file name\n"
						"--version , -v \n version number\n"
						"--help , -h \n print help information\n", argv[0]);
				return 0;
			default:
				fprintf(stderr, "please input options\n");
				break;

		}
	}

	if(optind < argc) {
		fprintf(stderr, "non-option ARGV-elements:");
		while(optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}

	if(NULL == conf) {
		fprintf(stderr, "please input config file path\n");
		return -1;
	}

	printf("load config file success!\n");
	printf("the file is : %s\n", conf);

	return 0;
}


#define array_size 50
#define ECHOMAX 20

int max_clnt, port;
char *type_proto;

struct mem {
	char memTotal[array_size];	//
	char memFree[array_size];	// ОЗУ
	char memAvail[array_size];	//
};

struct info {
	char net_int[array_size][array_size];	  // Сетевые интерфейсы
	int count;			  // счетчик кол-ва сетев. инетерфейсов
	char cpuavg[array_size];		  // Нагрузка процессора
	struct mem mem;		  // ОЗУ
};



#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MONOCUT_MAX_PWR {300, 300, 200}
#define MONOCUT_MIN_PWR {0, 0, 0}

#define MONOMIX_MAX_PWR {220, 180, 140}
#define MONOMIX_MIN_PWR {0, 0, 0}

#define MONOCOAG_MAX_PWR {120, 120, 120}
#define MONOCOAG_MIN_PWR {0, 0, 0}

#define BICUT_MAX_PWR {300, 300, 200}
#define BICUT_MIN_PWR {0, 0, 0};

#define BIMIX_MAX_PWR {220, 180, 140}
#define BIMIX_MIN_PWR {0, 0, 0}

#define BICOAG_MAX_PWR {200, 150, 150}
#define BICOAG_MIN_PWR {0, 0, 0}

#define MAX_TIMEOUT 100
#define MIN_TIMEOUT 0

#define PWR_STEP 10
#define TIMEOUT_STEP 1

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H_

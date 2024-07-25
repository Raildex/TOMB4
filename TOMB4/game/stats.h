#ifndef TOMB4_GAME_STATS_H
#define TOMB4_GAME_STATS_H
typedef struct STATS {
	unsigned long Timer;
	unsigned long Distance;
	unsigned long AmmoUsed;
	unsigned long AmmoHits;
	unsigned short Kills;
	unsigned char Secrets;
	unsigned char HealthUsed;
} STATS;
#endif// TOMB4_GAME_STATS_H

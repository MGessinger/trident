#include <stdio.h>
#include <stdlib.h>
#include "targets.h"

target_t * newSingleTarget (char * name, target_type type)
{
	target_t * T = malloc(sizeof(target_t));
	if (T == NULL)
		return NULL;

	T->name = name;
	T->type = type;
	T->file = -1;
	T->filter = NULL;
}

target_t * addFilter (target_t * T, regex_t * filter)
{
	if (T == NULL)
	{
		regfree(filter);
		return NULL;
	}

	T->filter = filter;
	return T;
}

void clearTarget (target_t T)
{
	free(T.name);
	if (T.filter != NULL)
	{
		regfree(T.filter);
		free(T.filter);
	}
}

targets_t * newTargets (target_t * T)
{
	targets_t * Ts = malloc(sizeof(targets_t));
	if (Ts == NULL)
		return NULL;

	Ts->alloc = 4;
	Ts->targets = malloc(4 * sizeof(target_t));
	if (Ts->targets == NULL)
	{
		free(Ts);
		free(T);
		perror("newTargets: alloc");
		return NULL;
	}
	Ts->targets[0] = *T;
	Ts->length = 1;
	free(T);

	return Ts;
}

targets_t * appendTarget (targets_t * Ts, target_t * T)
{
	if (Ts->targets == NULL)
		return Ts;
	Ts->targets[Ts->length] = *T;
	Ts->length ++;
	free(T);

	if (Ts->length == Ts->alloc)
	{
		Ts->targets = realloc(Ts->targets, Ts->alloc * 2 * sizeof(target_t));
		if (Ts->targets = NULL)
		{
			perror("appendTarget: realloc");
			free(Ts);
			return NULL;
		}
		else
			Ts->alloc *= 2;
	}

	return Ts;
}

void clearTargets (targets_t * Ts)
{
	if (Ts == NULL)
		return;

	for (int i = 0; i < Ts->length; i++)
		clearTarget(Ts->targets[i]);
	free(Ts->targets);
	free(Ts);
}

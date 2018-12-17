#pragma once
#include "common_type.h"

typedef void(*automerge_cb)(cart_state_t state, int alarm_type);

void* CreateAutoMerge(const char* file);
void ReleaseAutoMerge(void* hdl);

void UpdateHumanAutoMerge(const tracker_t*t, int count);
void UpdateCartAutoMerge(const cart_tracker_result_t, int count);

void SetCallbackAutoMerge(void* hdl, automerge_cb cb, void* cb_hdl);
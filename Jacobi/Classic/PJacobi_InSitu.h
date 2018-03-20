
/* Data Access Function prototypes */
void LibSimInitialize(simulation_data *sim, int argc, char *argv[]);
visit_handle SimGetMetaData(void *);
visit_handle SimGetMesh(int, const char *, void *);
visit_handle SimGetVariable(int, const char *, void *);
visit_handle SimGetDomainList(const char *, void *);
void ControlCommandCallback(const char *cmd, const char *args, void *cbdata);
void SlaveProcessCallback(void *cbdata);
int ProcessVisItCommand(simulation_data *sim);
void SetupCallbacks(simulation_data *sim);
void Create_InSitu_Outputs(simulation_data *sim);
void mainloop_batch(simulation_data *sim);
void mainloop_interactive(simulation_data *sim);

#ifdef PARALLEL
int visit_broadcast_int_callback(int *value, int sender, void *cbdata);
int visit_broadcast_string_callback(char *str, int len, int sender, void *cbdata);
#endif

#ifndef OPEN_JTALK_CPP_H
#define OPEN_JTALK_CPP_H

struct OpenJTalk;
extern const char* OpenJTalk_GetLastError(OpenJTalk** openjtalk);
extern int OpenJTalk_Create(OpenJTalk** openjtalk,int argc,const char **argv);
extern int OpenJTalk_synthesis_toplay(OpenJTalk** openjtalk,MecabThreadFeature* mecabfeature);
extern int OpenJTalk_synthesis_toplay_savefile(OpenJTalk** openjtalk,MecabThreadFeature* mecabfeature,const char* filename);
extern int OpenJTalk_Delete(OpenJTalk** openjtalk);

#endif /* OPEN_JTALK_CPP_H */

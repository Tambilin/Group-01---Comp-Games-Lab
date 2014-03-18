#include "soundeffect.h"

int soundeffect::endWithError(char* msg)
{
    //Display error message in console
	int error=0;
    cout << msg << "\n";
    system("PAUSE");
    return error;
}

int soundeffect::createSound(char * filename, int arg)
{  
	
    //Loading of the WAVE file
    FILE *fp = NULL;                                                            //Create FILE pointer for the WAVE file
    fp=fopen(filename,"rb");                                            //Open the WAVE file
    if (!fp) return endWithError("Failed to open file");                        //Could not open file
    
    //Variables to store info about the WAVE file (all of them is not needed for OpenAL)
    char type[4];
    DWORD size,chunkSize;
    short formatType,channels;
    DWORD sampleRate,avgBytesPerSec;
    short bytesPerSample,bitsPerSample;
    DWORD dataSize;
    
    //Check that the WAVE file is OK
    fread(type,sizeof(char),4,fp);                                              //Reads the first bytes in the file
    if(type[0]!='R' || type[1]!='I' || type[2]!='F' || type[3]!='F')            //Should be "RIFF"
    return endWithError ("No RIFF");                                            //Not RIFF

    fread(&size, sizeof(DWORD),1,fp);                                           //Continue to read the file
    fread(type, sizeof(char),4,fp);                                             //Continue to read the file
    if (type[0]!='W' || type[1]!='A' || type[2]!='V' || type[3]!='E')           //This part should be "WAVE"
    return endWithError("not WAVE");                                            //Not WAVE
    
    fread(type,sizeof(char),4,fp);                                              //Continue to read the file
    if (type[0]!='f' || type[1]!='m' || type[2]!='t' || type[3]!=' ')           //This part should be "fmt "
    return endWithError("not fmt ");                                            //Not fmt 
    
    //Now we know that the file is a acceptable WAVE file
    //Info about the WAVE data is now read and stored
    fread(&chunkSize,sizeof(DWORD),1,fp);
    fread(&formatType,sizeof(short),1,fp);
    fread(&channels,sizeof(short),1,fp);
    fread(&sampleRate,sizeof(DWORD),1,fp);
    fread(&avgBytesPerSec,sizeof(DWORD),1,fp);
    fread(&bytesPerSample,sizeof(short),1,fp);
    fread(&bitsPerSample,sizeof(short),1,fp);
    
    fread(type,sizeof(char),4,fp);
    if (type[0]!='d' || type[1]!='a' || type[2]!='t' || type[3]!='a')           //This part should be "data"
    return endWithError("Missing DATA");                                        //not data
    
    fread(&dataSize,sizeof(DWORD),1,fp);                                        //The size of the sound data is read
    
    //Display the info about the WAVE file
    cout << "Chunk Size: " << chunkSize << "\n";
    cout << "Format Type: " << formatType << "\n";
    cout << "Channels: " << channels << "\n";
    cout << "Sample Rate: " << sampleRate << "\n";
    cout << "Average Bytes Per Second: " << avgBytesPerSec << "\n";
    cout << "Bytes Per Sample: " << bytesPerSample << "\n";
    cout << "Bits Per Sample: " << bitsPerSample << "\n";
    cout << "Data Size: " << dataSize << "\n";
    
    buf[arg]= new unsigned char[dataSize];                            //Allocate memory for the sound data
    cout << fread(buf[arg],sizeof(BYTE),dataSize,fp) << " bytes loaded\n";           //Read the sound data and display the 
                                                                                //number of bytes loaded.
                                                                                //Should be the same as the Data Size if OK
    

    ALuint frequency=sampleRate;                                               //The Sample Rate of the WAVE file
    ALenum format=0;                                                            //The audio format (bits per sample, number of channels)
    
	//alGenBuffers(NUM_BUFFERS, buffer); 
    alGenBuffers(1, &buffer[arg]);                                                    //Generate one OpenAL Buffer and link to "buffer"
    alGenSources(1, &source[arg]);                                                   //Generate one OpenAL Source and link to "source"
	//alGenBuffers(NUM_BUFFERS, buffer);                                                    //Generate one OpenAL Buffer and link to "buffer"
    //alGenSources(NUM_SOURCES, source);                                                   //Generate one OpenAL Source and link to "source"
    if(alGetError() != AL_NO_ERROR) return endWithError("Error GenSource");     //Error during buffer/source generation
    
    //Figure out the format of the WAVE file
    if(bitsPerSample == 8)
    {
        if(channels == 1)
            format = AL_FORMAT_MONO8;
        else if(channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if(bitsPerSample == 16)
    {
        if(channels == 1)
            format = AL_FORMAT_MONO16;
        else if(channels == 2)
            format = AL_FORMAT_STEREO16;
    }
    if(!format) return endWithError("Wrong BitPerSample");                      //Not valid format

    alBufferData(buffer[arg], format, buf[arg], dataSize, frequency);                    //Store the sound data in the OpenAL Buffer
    if(alGetError() != AL_NO_ERROR) 
    return endWithError("Error loading ALBuffer");                              //Error during buffer loading
  
    //Sound setting variables
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };                                    //Position of the source sound
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };                                    //Velocity of the source sound
    ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };                                  //Position of the listener
    ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };                                  //Velocity of the listener
    ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };                 //Orientation of the listener
                                                                                //First direction vector, then vector pointing up) 
    //Listener                                                                               
    alListenerfv(AL_POSITION,    ListenerPos);                                  //Set position of the listener
    alListenerfv(AL_VELOCITY,    ListenerVel);                                  //Set velocity of the listener
    alListenerfv(AL_ORIENTATION, ListenerOri);                                  //Set orientation of the listener
    
    //Source
    alSourcei (source[arg], AL_BUFFER,   buffer[arg]);                                 //Link the buffer to the source
    alSourcef (source[arg], AL_PITCH,    1.0f     );                                 //Set the pitch of the source
    alSourcef (source[arg], AL_GAIN,     1.0f     );                                 //Set the gain of the source
    alSourcefv(source[arg], AL_POSITION, SourcePos);                                 //Set the position of the source
    alSourcefv(source[arg], AL_VELOCITY, SourceVel);                                 //Set the velocity of the source
    alSourcei (source[arg], AL_LOOPING,  AL_FALSE );                                 //Set if source is looping sound
    
    //Clean-up
    fclose(fp);                                                                 //Close the WAVE file

    return EXIT_SUCCESS;                                                        
}

int soundeffect::play(int track){
    alSourcePlay(source[track]);                                                       //Play the sound buffer linked to the source
    if(alGetError() != AL_NO_ERROR) return endWithError("Error playing sound"); //Error when playing sound
	return 0;
}

void soundeffect::setLoop(int track, bool status){
	if(status == true){
		alSourcei (source[track], AL_LOOPING,  AL_TRUE );
	} else {
		alSourcei (source[track], AL_LOOPING,  AL_FALSE );
	}
}

void soundeffect::cleanup(int arg){
	delete buf[arg];
}

void soundeffect::toggleBackgroundSound(int track, bool enabled){
	if(enabled != started){
	if(enabled){
		setLoop(track, true);
		play(track);
		started = enabled;
	} else {
		setLoop(track, false);
		started = enabled;
	}
	}
}

int soundeffect::createDeviceContext(){
	//Now OpenAL needs to be initialized 
    ALCdevice *device;                                                          //Create an OpenAL Device
    ALCcontext *context;                                                        //And an OpenAL Context
    device = alcOpenDevice(NULL);                                               //Open the device
    if(!device) return endWithError("no sound device");                         //Error during device oening
    context = alcCreateContext(device, NULL);                                   //Give the device a context
    alcMakeContextCurrent(context);                                             //Make the context the current
    if(!context) return endWithError("no sound context");                       //Error during context handeling
	return 0;
}


soundeffect::soundeffect(void)
{
	createDeviceContext();
	started = false;
}


soundeffect::~soundeffect(void)
{
	alDeleteSources(1, source);                                                //Delete the OpenAL Source
    alDeleteBuffers(1, buffer);                                                 //Delete the OpenAL Buffer
    alcMakeContextCurrent(NULL);                                                //Make no context current
    //alcDestroyContext(context);                                                 //Destroy the OpenAL Context
    //alcCloseDevice(device);                                                     //Close the OpenAL Device
	//delete[] buf;                                                               //Delete the sound data buffer
}

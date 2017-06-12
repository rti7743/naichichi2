#pragma once

class VolumeControll
{
public:
	VolumeControll();
	virtual ~VolumeControll();

	void Create();
	unsigned int GetSpeakerVolume() const;
	unsigned int GetMicVolume() const;
	void ChangeMicVolume() ;
	void ChangeSpeakerVolume() ;

private:
	bool IsNightSpeakerVolume(const time_t& now) const;

	unsigned int NowSpeakerVolume;
	unsigned int NowMicVolume;
};

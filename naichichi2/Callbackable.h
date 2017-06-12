#pragma once

class CallbackPP;

//コールバックするデータを保持する
class CallbackPP
{
public:
	CallbackPP()
	{
		ObjectID = 0;
	}
	CallbackPP(const CallbackPP& obj)
	{
		this->ObjectID = obj.ObjectID;
		this->func = obj.func;
	}

	static CallbackPP New(std::function<void (void) > f);
	static CallbackPP NoCallback()
	{
		CallbackPP p;
		return p;
	}

	void fire() const
	{
		if (this->ObjectID == 0) return ;
		this->func();
	}

	bool operator==(const CallbackPP& o) const
	{
		return o.ObjectID == this->ObjectID;
	}
	bool operator!=(const CallbackPP& o) const
	{
		return o.ObjectID != this->ObjectID;
	}
	bool Empty() const
	{
		return this->ObjectID == 0 ;
	}
private:
	int ObjectID;
	std::function<void (void) > func;
};

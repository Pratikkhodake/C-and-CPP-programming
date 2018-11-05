# include"myheader.h"

class mask_fun
{
protected:

	UINT ino;
	int ipos;
	UINT iresult;

public:

	mask_fun();
	~mask_fun();
	void accept();
	void display();
	BOOL ChkBit();
	UINT OffBit();
	UINT OnBit();
};
	
class togglebit: public mask_fun
{
public:
	UINT ToggleBit();
	UINT Toggle9Bit();
};



// constructor

mask_fun::mask_fun()
{
	ino = 0;
	ipos = 0;
}
mask_fun:: ~mask_fun()
{
	iresult = 0;
}

void mask_fun::accept()
{
	cout << "\n Please Enter the Number:\n";
	cin >> ino;
	cout << "\n enter the position:\n";
	cin >> ipos;
	
}
void mask_fun::display()
{
	cout << "\n Given number:"<<ino<<"\t"<<" \n position is : "<<  ipos;
}

// write a program which accept one number & one position from user & check whether a bit at that position is on or off. 

BOOL mask_fun:: ChkBit()
{
	UINT mask = 0x1;

	if ((ipos < 1) || (ipos > 32))
	{
		return ino;
	}

	mask = mask << (ipos - 1);
    iresult = ino & mask;
	
	if (iresult == mask)
	{
		return true;
	}
	else
	{
		return false;
	}

}

//  Write a program which accept one number & one position from user and OFF that Bit. Return the modified number.

UINT mask_fun::OffBit()
{
	UINT mask = 0x1;

	if ((ipos < 1) || (ipos > 32))
	{
		return ino;
	}

	mask = mask << (ipos - 1);
	mask = ~mask;
	iresult = ino & mask;

	return iresult;
}

// Write a program which accept one number & one position from user and ON that bit. Return the modified number. 

UINT mask_fun::OnBit()
{
	UINT mask = 0x1;

	if ((ipos < 1) || (ipos > 32))
	{
		return ino;
	}

	mask = mask << (ipos - 1);

	iresult = ino | mask;

	return iresult;

}

// Write a program which accept one number & one position from user and TOGGLE that bit. Return the modified number.

UINT togglebit::ToggleBit()
{
	UINT mask = 0x1;

	if ((ipos < 1) || (ipos > 32))
	{
		return ino;
	}

	mask = mask << (ipos - 1);

	iresult = ino ^ mask;

	return iresult;

}

// Write a program which accept one number from user and TOGGLE the contents of first and last nibble of the number. Return the modified number.

UINT togglebit::Toggle9Bit()
{
	cout << ino;
	UINT mask = 0xf000000f;

	iresult = ino ^ mask;

	return iresult;
}

int main()
{
	int bret = true, Cret = 0, Dret = 0, Fret = 0;
	
	mask_fun obj1;
	obj1.accept();
	obj1.display();

	cout << "\n";

	bret=obj1.ChkBit();
	
	if (bret == true)
	{
		cout << "\n BIT IS ON \n";
	}
	else
	{
		cout << "\n BIT IS OFF";
	}

	Cret = obj1.OffBit();

	cout << "\n OffBit Number is:" << Cret;

	cout << "\n";

	Dret = obj1.OnBit();

	cout << "\n OnBit Number is:" << Dret;

	cout << "\n";

	togglebit obj2;
	Fret = obj2.ToggleBit();

	cout << "\n Toggle Number is:" << Fret;

	cout << "\n";
	
	cout << "\n Toggle9Number is:\t" << obj2.Toggle9Bit();
	

	

	cout << "\n";
	system("pause");
	return 0;
}
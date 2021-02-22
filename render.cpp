#include <d3d9.h>
#include <d3dx9.h>
#include <cmath>
#include <vector>
#include "Shapes.h"
#include "render.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

//Class that describes a charged particle
class charge:public circle
{
private:
	double v_x,v_y; //particle velocity
	double pos_x,pos_y; //particle postion
	int mass;
public:
	int i_charge;
	charge(int X,int Y,int R,int Fidel,LPDIRECT3DDEVICE9 D3ddev,
		DWORD circ_color,double V_X,double V_Y,int MASS,int CHARGE):
		circle(X,Y,R,Fidel,D3ddev,circ_color)
	{
		v_x = V_X;
		v_y = V_Y;
		pos_x = X;
		pos_y = Y;
		mass = MASS;
		i_charge = CHARGE;
	}
	
	//Function that updates the velocity
	void calc(double force_x,double force_y)
	{
		v_x += force_x/mass;
		v_y += force_y/mass;
		if(pos_x < 0)
			v_x = abs(v_x);
		if(pos_x > 1920)
			v_x = -1*abs(v_x);
		if(pos_y < 0)
			v_y = abs(v_y);
		if(pos_y > 1080)
			v_y = -1*abs(v_y);
	}

	//Function that updates the position
	void move()
	{
		pos_x += v_x;
		pos_y += v_y;
		x = int(pos_x);
		y = int(pos_y);
	}
};

int random(int from,int to)
{
	return rand()%(to - from + 1) + from;
}

void render(LPDIRECT3DDEVICE9 d3ddev)
{
	static POINT mouse_pos;
	GetCursorPos(&mouse_pos);
	const int amnt = 100;
	const int K = 30;
	static std::vector<charge> charges;
	if(charges.empty())
	{
		for(int i = 0;i < amnt;i++)
		{
			if(random(0,1) == 1)
			{
				charge temp(random(950,970),random(530,550),2,4,d3ddev,
						D3DCOLOR_XRGB(0,255,0),0,0,10000,-6);
				charges.push_back(temp);
			}
			else
			{
				charge temp(random(950,970),random(530,550),2,4,d3ddev,
						D3DCOLOR_XRGB(0,255,0),0,0,10000,-6);
				charges.push_back(temp);
			}
		}
	}

	for(int i = 0;i < (amnt-1);i++)
	{
		for(int j = i + 1;j < amnt;j++)
		{
			double f_x;
			double f_y;
			double x = charges[i].get(X) - charges[j].get(X);
			double y = charges[i].get(Y) - charges[j].get(Y);
			double f = K*charges[i].i_charge*charges[j].i_charge/(pow(x,2)+pow(y,2));
			if(x == 0)
			{
				f_y = f;
				f_x = 0;
			}
			else
			{
				f_x = f/sqrt(1+pow(double(y/x),2));
				f_y = abs(y)*f_x/abs(x);
			}

			if(charges[i].get(X) > charges[j].get(X))
			{
				if(charges[i].get(Y) > charges[j].get(Y))
				{
					charges[i].calc(f_x,f_y);
					charges[j].calc(-1*f_x,-1*f_y);
				}
				else
				{
					charges[i].calc(f_x,-1*f_y);
					charges[j].calc(-1*f_x,f_y);
				}
			}
			else
			{
				if(charges[i].get(Y) > charges[j].get(Y))
				{
					charges[i].calc(-1*f_x,f_y);
					charges[j].calc(f_x,-1*f_y);
				}
				else
				{
					charges[i].calc(-1*f_x,-1*f_y);
					charges[j].calc(f_x,f_y);
				}
			}
		}
	}

	if(KEY_DOWN(VK_LBUTTON))
	{
		for(int i = 0;i < amnt;i++)
		{
			if(charges[i].i_charge > 0)
			{
				double f_x;
				double f_y;
				double x = mouse_pos.x - charges[i].get(X);
				double y = mouse_pos.y - charges[i].get(Y);
				double f = 30*charges[i].i_charge/(pow(x,2)+pow(y,2));
				if(x == 0)
				{
					f_y = f;
					f_x = 0;
				}
				else
				{
					f_x = f/sqrt(1+pow(y/x,2));
					f_y = abs(y)*f_x/abs(x);
				}
				if(mouse_pos.x < charges[i].get(X))
					f_x = -1*f_x;
				if(mouse_pos.y < charges[i].get(Y))
					f_y = -1*f_y;
				charges[i].calc(f_x,f_y);
			}
		}
	}

	for(int i = 0;i < amnt;i++)
	{
		charges[i].move();
		charges[i].draw();
	}
}

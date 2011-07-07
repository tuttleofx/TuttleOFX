#ifndef _channels_hpp_
#define _channels_hpp_

#include <assert.h>
#include <boost/gil/gil_all.hpp>
#include <boost/function.hpp>

struct channels
{
	struct channel
	{
		int pos;
		channel* next;
	};

	channel* head;
	channel* current;

	channels() : head(NULL), current(NULL) {}
	channels(channel* head) : head(head), current(head) {}
	operator bool (){return current != NULL;}
	operator channel * () {return head;}
	double operator *(){return current->pos;}
	void operator ++(int){current = current->next;}
	void operator ++(){current = current->next;}
	void reset() {current = head;}

	~channels()
	{
		while (head)
		{
			channel* tmp = head->next;
			free(head);
			head = tmp;
		}
		
		head = NULL;
	}

	channels(int width, int channels)
	{
		assert(channels);
		assert(width);
	
		head = (channel*)malloc(sizeof(channel));
		
		head->pos = width-1;
		head->next = NULL;
		
		if (channels != 1)
		{
			head->pos = -1;
			channel* save_head = head;
			
			--channels;	
			int adj = width/channels;
			int extra = 0;
			if (width % channels)
				extra = width-adj*channels;
		
			while (head->pos < width-1)
			{
				int add = adj;
				if (extra-- > 0)
					add++;
		
				channel* tail  = (channel*)malloc(sizeof(channel));
				tail->pos = head->pos + add;
				tail->next = NULL;
		
				head->next = tail;
				head = tail;
			}
		
			head = save_head;
			head->pos = 0;

			current = head;
		}
	}
};


namespace layer
{

//Used by the horizontal date labels
template <typename view_t>
struct horizontal_channels
{
	typedef boost::function<void (view_t&)> layer_t;
	std::vector<layer_t> layers;
	int periods;

	horizontal_channels(layer_t* p, int size, int periods) : periods(periods)
	{
		for (int n = 0; n < size; ++n)
			layers.push_back(p[n]);
	}

	void operator()(view_t& view)
	{
		using namespace boost::gil;

		channels i(periods, layers.size());
		channels x(view.width(), periods);

		int n=0, m=0;
		for (; i; ++i, ++m)
		{
			for (; n < *i; ++n, ++x);

			view_t v = subimage_view(view,*x,0,0,view.height());
			layers[m](v);
		}
	}
};

}

#endif

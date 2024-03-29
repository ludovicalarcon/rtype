#include "buffer.h"
#include "utils.hpp"
#include <cmath>

namespace hpl
{
	const ulint	Buffer::_bufferSize = 256;

	Buffer::RotatingBuffer::RotatingBuffer(ulint size) : _deb(0), _end(0), _size(size), _buff(NULL), _lenght(0)
	{
		_buff = new char[size];
	}
	Buffer::RotatingBuffer::RotatingBuffer(RotatingBuffer const &copy) : _buff(NULL), _lenght(0)
	{
		*this = copy;
	}
	Buffer::RotatingBuffer::~RotatingBuffer(void) { if (_buff) delete[] _buff; }

	void	Buffer::RotatingBuffer::read(char *buff, ulint size) const
	{
		ulint	toRead = Operators::Min(_size, size);
		ulint	deb = 0;
		while (deb < toRead)
		{
			*buff = readChar(deb);
			++buff;
			++deb;
		}
	}
	void	Buffer::RotatingBuffer::get(char *buff, ulint size)
	{
		ulint	toRead = Operators::Min(_size, size);
		while (toRead)
		{
			*buff = getChar();
			++buff;
			--toRead;
		}
	}
	void	Buffer::RotatingBuffer::write(char const *buff, ulint size)
	{
		ulint	toWrite = Operators::Min(_size - this->size(), size);
		while (toWrite)
		{
			addChar(*buff);
			++buff;
			--toWrite;
		}
	}

	ulint	Buffer::RotatingBuffer::size(void) const
	{
		return (_lenght);
	}
	ulint	Buffer::RotatingBuffer::sizeMax(void) const { return (_size); }

	void	Buffer::RotatingBuffer::addChar(char c)
	{
		_buff[_end] = c;
		_end = (_end + 1) % _size;
		++_lenght;
	}

	char	Buffer::RotatingBuffer::readChar(ulint it) const
	{
		return (_buff[(_deb + it) % _size]);
	}

	char	Buffer::RotatingBuffer::getChar(void)
	{
		char	c = _buff[_deb];
		_deb = (_deb + 1) % _size;
		--_lenght;
		return (c);
	}

	void	Buffer::RotatingBuffer::clear(void) { _deb = _end; _lenght = 0; }

	Buffer::RotatingBuffer	&Buffer::RotatingBuffer::operator=(Buffer::RotatingBuffer const &copy)
	{
		if (_buff)
			delete[] _buff;
		_size = copy._size;
		_buff = new char[_size];
		_deb = copy._deb;
		_end = copy._end;
		_lenght = copy._lenght;
		for (ulint it = 0; it < _lenght; ++it)
			_buff[(_deb + it) % _size] = copy._buff[(_deb + it) % _size];
		return (*this);
	}

	Buffer::Buffer(void) : _size(0) { _buffers.emplace_back(_bufferSize); }
	Buffer::Buffer(char *buff, ulint size) : _size(0)
	{
		_buffers.emplace_back(_bufferSize);
		write(buff, size);
	}
	Buffer::Buffer(Buffer const &copy) : _size(0) { *this = copy; }
	Buffer::~Buffer(void) { _buffers.clear(); }

	ulint	Buffer::read(char *buff, ulint size) const
	{
		_locker.lock();
		ulint			wSize;
		auto			it = _buffers.begin();
		ulint			total = 0;

		while (size && it != _buffers.end())
		{
			wSize = Operators::Min(size, it->size());
			it->read(buff, wSize);
			size -= wSize;
			buff += wSize;
			total += wSize;
			++it;
		}
		_locker.unlock();
		return (total);
	}
	ulint	Buffer::get(char *buff, ulint size)
	{
		_locker.lock();
		ulint			wSize;
		auto			it = _buffers.begin();
		ulint			total = 0;

		while (size && it != _buffers.end())
		{
			wSize = Operators::Min(size, it->size());
			it->get(buff, wSize);
			size -= wSize;
			buff += wSize;
			total += wSize;
			if (_buffers.size() == 1)
				++it;
			else
				it = _buffers.erase(it);
		}
		_size -= total;
		_locker.unlock();
		return (total);
	}
	void	Buffer::write(Buffer const &buff)
	{
		_locker.lock();
		ulint			wSize;
		char			content[256];
		Buffer			tmp(buff);

		while (tmp.size())
		{
			wSize = tmp.get(content, sizeof(content));
			write(content, wSize);
		}
		_locker.unlock();
	}
	void	Buffer::write(char const *buff, ulint size)
	{
		_locker.lock();
		ulint			wSize;
		RotatingBuffer	*current;
		_size += size;

		while (size)
		{
			current = &_buffers.back();
			wSize = Operators::Min(current->sizeMax() - current->size(), size);
			current->write(buff, wSize);
			size -= wSize;
			buff += wSize;
			if (size)
				_buffers.emplace_back(_bufferSize * (ulint)std::pow(2, _buffers.size()));
		}
		_locker.unlock();
	}

	ulint	Buffer::size(void) const { return (_size); }

	void	Buffer::clear(void)
	{
		_locker.lock();
		for (auto it = _buffers.begin(); it != _buffers.end(); ++it)
			it->clear();
		_size = 0;
		_locker.unlock();
	}

	Buffer	&Buffer::operator=(Buffer const &copy)
	{
		clear();
		_locker.lock();
		for (auto it = copy._buffers.begin(); it != copy._buffers.end(); ++it)
		{
			_buffers.emplace_back(_bufferSize * (ulint)std::pow(2, _buffers.size()));
			_buffers.back() = *it;
		}
		_size = copy._size;
		return (*this);
		_locker.unlock();
	}
}

#pragma once
#include "../es_config.h"
#include <assert.h>

#include "../check_function.h"

BEGIN_ES_NAMESPACE

	class bin_buf_exception : public ::std::exception
	{
	public:
		bin_buf_exception(const char *const& _What) : ::std::exception(_What) {}
	};


	// ָ���ڴ�ռ��buf. ��size�ǹ̶���. ֻ��дsize��Χ�ڵ�����,�����׳��쳣. 
	//
	template <check_fuction buf_check = _assert_check>
	class _appointed_buf
	{
	public:
		friend class bufTester;
		friend class istreamTester_appointed_buf;

		_appointed_buf(char *_Ptr, size_t bufSize) 
			: m_Ptr(_Ptr), m_BufSize(bufSize)
		{
			assert(bufSize <= INT_MAX); // ����ж�ʹ��int��ΪbufSizeת�ʹ���������ʱ�Ŀ��ܴ���intΪ������תΪsize_t�󣬻��ɷǳ����������
		}

		template <class left_buf_type> // ��������������buf����Ҫʵ�������ӿ�: c_str()  size().
		_appointed_buf& operator = (left_buf_type& b);

		size_t size(void) const {return m_BufSize;}

		void resize(size_t /*_Newsize*/){do_check(buf_check, false, "���ܸı�_appointed_buf��size!");} // ����ӿڲ��ܵ��õ�.
		const char* c_str() const {return m_Ptr;}
		char& operator[](size_t _Off){
			do_check(buf_check, _Off < m_BufSize, "_appointed_buf::operator[] ������index����bufsize!");
			return m_Ptr[_Off];
		}

		const char& operator[](size_t _Off) const{
			do_check(buf_check, _Off < m_BufSize, "_appointed_buf::operator[] ������index����bufsize!");
			return m_Ptr[_Off];
		}


		// replace [_Off, _Off + _Count) with [_Ptr, _Ptr + _Count)
		_appointed_buf& replace(size_t _Off, const char *_Ptr, size_t _Count){
			assert(_Ptr != NULL); // ����ʹ��assert , ��������϶��Ǵ���д����.

			do_check(buf_check, _Off < m_BufSize, "_appointed_buf::replace �����ĸ���λ����buf����Ч��Χ֮��!");
			do_check(buf_check, _Count >= 0, "_appointed_buf::replace �븲�ǵĳ���С��0!");
			do_check(buf_check, _Count <= m_BufSize, "_appointed_buf::replace ָ�����ǵĳ���̫��!"); // ����ܹ���æ�ж� int���͸������������󣬱�ɷǳ�������������⡣
			do_check(buf_check, _Off + _Count <= m_BufSize, "_appointed_buf::replace ���븲�ǵķ�Χ������buf����Ч��Χ!");

			const size_t leftSize = m_BufSize - _Off;
			memcpy_s(&m_Ptr[_Off], leftSize, _Ptr, _Count);
			return *this;
		}


		// ֻ������ݣ����ı��С��
		void clear(void){
			memset(m_Ptr, NULL, m_BufSize);
		}



	private:
		// ��Ҫʱ����ʵ������ӿ�
		_appointed_buf(_appointed_buf& b);

		// ����Ľӿڹ�������ʹ��
		//
		_appointed_buf();
		_appointed_buf(char* p);



	private:
		char* const m_Ptr; // ֪����ϵ. ���û�ͨ�����캯��ָ��һ�����е��ڴ�ռ�.
		const size_t m_BufSize;
	};

	template <check_fuction buf_check>
		template <class left_buf_type>
		_appointed_buf<buf_check>& _appointed_buf<buf_check>::operator = (left_buf_type& b)
		{
			if (this == &b)
				return *this;

			do_check(buf_check, m_BufSize >= b.m_BufSize, "");
			assert(m_Ptr != NULL); // ��������϶��Ǵ���д����.
			assert(b.m_BufSize >= 0);

			memset(m_Ptr, NULL, m_BufSize);
			memcpy_s(m_Ptr, m_BufSize, b.m_Ptr, b.m_BufSize);
			return *this;
		}

END_ES_NAMESPACE

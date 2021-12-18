#pragma once
#include "../es_config.h"
#include <assert.h>

//#include "boost/type_traits/is_fundamental.hpp"
//#include "boost/type_traits/is_pod.hpp"
//#include "boost/mpl/assert.hpp"

#include "binary_stream_def.h"
#include "../check_function.h"


BEGIN_ES_NAMESPACE

	template <class buf_type, check_fuction ostream_check = _assert_check>
	class _binary_ostream
	{
	public:
		friend class ostreamTester;
		friend class iostreamTester;
		friend class ostreamTester_appointed_buf;

		_binary_ostream(buf_type &buf) : m_WriteBuf(buf), m_WritePos((int)buf.size()) {}
		virtual ~_binary_ostream(void) {}

	public:

		// ������ѹ������, λ����writePosָ��.
		// ���δ����WritePos, Ĭ��״̬WritePos���Ǵ�β, ����buf���������ݵĺ��濪ʼд.
		// ���ص�ǰ��writePos.
		//
		int Pack(const char* pBuf, int size);

		// ����ӿ�ֻ����POD���� (int, char, DWORD��, �Լ�ֻ������Щ�������͵Ľṹ�����) ���ܵ���.
		// ���ڷ�POD����,��������֮.
		template <class pack_type>
			int Pack(const pack_type& buf);

		int PackString(const ::std::string& str) {
			return _PackString(str);
		}

		int PackWString(const ::std::wstring& wstr) {
			::std::string str(reinterpret_cast<const char *>(wstr.c_str()), wstr.size() * sizeof(wchar_t));
			return _PackString(str);
		}

	public:
		int GetWritePos(void);

		// ����д���. ������buf����Ч���ݷ�Χ��,�������쳣.
		void SetWritePos(int writePos);

		// �ӵ�ǰ�α�λ���ƶ��α�. offset < 0��ʾ�����ƶ�, >0��ʾ�����ƶ�.
		// �ƶ����Ƿ�λ��ʱ,���׳��쳣
		void MoveWritePos(int offset);

		// ������Ч����֮���һ��λ�� : �� buf = "123" �� EndPos() == 3
		int EndPos(void) const {return BufLen();}

	protected:
		_binary_ostream(const _binary_ostream&); // ��������

	private:
		_binary_ostream& operator=(const _binary_ostream&); // ��������


		int BufLen(void) const {return static_cast<int>(m_WriteBuf.size());}

		void ResizeBuf(int newSize);

		int _PackString(const ::std::string& buf);

	private:
		buf_type& m_WriteBuf; // ֪����ϵ��
		int m_WritePos;
	};

	//class binary_buf;
	//typedef _binary_ostream<binary_buf> binary_ostream;



	// **********************************************************************************************
	// **********************************************************************************************
	// ������ʵ��
	// **********************************************************************************************
	// **********************************************************************************************
	//
	template<class buf_type, check_fuction ostream_check>
		template <class pack_type>
			int _binary_ostream<buf_type, ostream_check>::Pack(const pack_type& buf)
			{
				// �����������ֻ��POD����, ����û�����Ƶ�ָ��
				//BOOST_MPL_ASSERT_MSG(boost::is_pod<pack_type>::value, obj_type_is_not_pod_type, (pack_type));

				// �����������ָ��.
				//BOOST_MPL_ASSERT_MSG(!(boost::is_pointer<pack_type>::value), obj_type_is_not_pod_type, (pack_type));

				// to do : ���ڽṹ���������ʹ�õ�ָ��,��û�����Ƶ�.
				// mpl_assert(pointer in struct or class);

				return Pack(reinterpret_cast<const char*>(&buf), sizeof(pack_type));
			}


	template<class buf_type, check_fuction ostream_check>
		int _binary_ostream<buf_type, ostream_check>::Pack( const char* pBuf, int size )
		{
			assert(pBuf != NULL);
			do_check(ostream_check, size >= 0, "call pack, but size < 0");

			int BufNeedLen =  GetWritePos() + size;
			if (BufNeedLen > BufLen())
				ResizeBuf(BufNeedLen);

			m_WriteBuf.replace(GetWritePos(), pBuf, size);
			SetWritePos(GetWritePos() + size);

			return GetWritePos();
		}

	template<class buf_type, check_fuction ostream_check>	
		int _binary_ostream<buf_type, ostream_check>::_PackString( const ::std::string& str )
		{
			Pack<int>((int)str.size());
			return Pack(str.c_str(), (int)str.size());
		}

	template<class buf_type, check_fuction ostream_check>
		void _binary_ostream<buf_type, ostream_check>::ResizeBuf( int newSize )
		{
			m_WriteBuf.resize(newSize);
		}


	template<class buf_type, check_fuction ostream_check>
		int _binary_ostream<buf_type, ostream_check>::GetWritePos( void )
		{
			return m_WritePos;
		}


	template<class buf_type, check_fuction ostream_check>
		void _binary_ostream<buf_type, ostream_check>::SetWritePos( int writePos )
		{
			do_check(ostream_check, writePos >= 0 && writePos <= EndPos(), "can't set writePos after context bound!");
			m_WritePos = writePos;
		}

	template<class buf_type, check_fuction ostream_check>
		void _binary_ostream<buf_type, ostream_check>::MoveWritePos( int offset )
		{
			int newPos = GetWritePos() + offset; // ʡ����������
			SetWritePos(newPos);
		}

END_ES_NAMESPACE

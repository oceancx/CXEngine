#pragma once
#include "../es_config.h"

#include <assert.h>

//#include "boost/type_traits/is_fundamental.hpp"
//#include "boost/type_traits/is_pod.hpp"
//#include "boost/mpl/assert.hpp"

#include "binary_stream_def.h"
#include "../check_function.h"

BEGIN_ES_NAMESPACE

	template <class buf_type, check_fuction istream_check = _assert_check>
	class _binary_istream
	{
	public:
		friend class istreamTester;
		friend class iostreamTester;
		friend class istreamTester_appointed_buf;

		_binary_istream(const buf_type &buf) : m_ReadBuf(buf), m_ReadPos(0) {}
		virtual ~_binary_istream(void) {}

	public:

    ::std::string Unpack(int sizeToUnpack);

		// �����ж�������, ��ȡλ����m_ReadPosָ��
		// �κ�����¶��ὫpBuf���(����������bufSizeInBytes��С),��д����. 
		// ����ȡʧ��ʱ(ʣ�����ݲ�������Ҫ�Ĵ�С, ���û�ָ����buf�����ȡ���ֽ���С), throw�쳣 or assert
		//
		void Unpack(char* pBuf, int bufSizeInBytes, int sizeToCpy);

		// ����ӿ�ֻ����POD���� (int, char, DWORD��, �Լ�ֻ������Щ�������͵Ľṹ�����) ���ܵ���.
		// ���ڷ�POD����,��������֮.
		template <class unpack_type>
			unpack_type Unpack(void);

		::std::string UnpackString(void) { // ��ǰ��4���ֽ���int�洢����,�����������.
	   		::std::string dst;
				UnpackString(dst);
				return dst;
		}

		::std::wstring UnpackWString(void)	{ // ��ǰ��4���ֽ���int�洢����,�����������.
				::std::string dst;
				UnpackString(dst);
				return ::std::wstring(reinterpret_cast<const wchar_t *>(dst.c_str()), dst.size() / sizeof(wchar_t));
		}

		//template <>
		//	buf_type Unpack(void)	{
		//		::std::string rstStr = Unpack<::std::string>(); // �ǽ�buf����::std::string���д洢��.
		//		return buf_type(rstStr.c_str(), rstStr.size());
		//	}

	public:

        size_t LeftBytes(void) const {return EndPos() - GetReadPos();}
		int GetReadPos(void) const;

		// ���ö�ȡ��. ������buf����Ч���ݷ�Χ��,�������쳣.
		void SetReadPos(int readPos);

		// �ӵ�ǰ�α�λ���ƶ��α�. offset < 0��ʾ�����ƶ�, >0��ʾ�����ƶ�.
		// �ƶ����Ƿ�λ��ʱ,���׳��쳣
		void MoveReadPos(int offset);

		// ������Ч����֮���һ��λ�� : �� buf = "123" �� EndPos() == 3.  
		//     ��readPos == endPosʱ����ʾ��ȡ��ĩβ��(������unpack��)��
		int EndPos(void) const;

	protected:
		_binary_istream(const _binary_istream& b) {}// ��������

	private:
		_binary_istream& operator=(const _binary_istream&); // ��������



		void UnpackString(::std::string& dstStr);

	private:
		// �ڲ��ӿ�: �����ж�������, ��ȡλ�����û�ָ��(readPos), ��ǰm_ReadPosλ��. 
		// ����ı����еĶ�ȡ�α�λ��, ������Ӱ�����Unpack�Ľ��.
		// �κ�����¶��ὫpBuf���(����������bufSizeInBytes��С),��д����. 
		// ����ȡʧ��ʱ(ʣ�����ݲ�������Ҫ�Ĵ�С, ���û�ָ����buf�����ȡ���ֽ���С), throw�쳣 or assert
		//
		void Read(char* pBuf, int bufSizeInBytes, int readPos, int sizeToRead) const;

		template <class read_type>
			read_type Read(int readPos) const;

		void Read(char* pBuf, int bufSizeInBytes, int sizeToRead) const { // �ӵ�ǰm_ReadPos��
			Read(pBuf, bufSizeInBytes, GetReadPos(), sizeToRead);
		}

		template <class read_type>
			read_type Read() const { // �ӵ�ǰm_ReadPos��
				return Read<read_type>(m_ReadPos);
			}


	private:
		const buf_type& m_ReadBuf; // ֪����ϵ.
		int m_ReadPos;
	};

	//class binary_buf;
	//typedef _binary_istream<binary_buf, throw> net_istream;



	// **********************************************************************************************
	// **********************************************************************************************
	// ������ʵ��
	// **********************************************************************************************
	// **********************************************************************************************
	//
	template <class buf_type, check_fuction istream_check>
        ::std::string _binary_istream<buf_type, istream_check>::Unpack(int sizeToUnpack)
        {
            if (sizeToUnpack < 1024) 
            {
                char buf[1024] = "";
                Unpack(buf, sizeToUnpack, sizeToUnpack);
                return ::std::string(buf);
            }
            else
            {
                char* buf = new char[sizeToUnpack + 1];
                Unpack(buf, sizeToUnpack, sizeToUnpack);
                ::std::string ret(buf);
                delete [] buf;
                return ret;
            }
        }

	template <class buf_type, check_fuction istream_check>
		void _binary_istream<buf_type, istream_check>::Unpack(char* pBuf, int bufSizeInBytes, int sizeToCpy)
		{
			Read(pBuf, bufSizeInBytes, sizeToCpy);
			MoveReadPos(sizeToCpy);
		}

	template <class buf_type, check_fuction istream_check>
		template <class unpack_type>
			unpack_type _binary_istream<buf_type, istream_check>::Unpack(void)
			{
				// �����������ֻ��POD����, ����û�����Ƶ�ָ��
				//BOOST_MPL_ASSERT_MSG(boost::is_pod<unpack_type>::value, obj_type_is_not_pod_type, (unpack_type));

				// �����������ָ��.
				//BOOST_MPL_ASSERT_MSG(!(boost::is_pointer<unpack_type>::value), obj_type_is_not_pod_type, (unpack_type));

				// to do : ���ڽṹ���������ʹ�õ�ָ��,��û�����Ƶ�.
				// mpl_assert(pointer in struct or class);

				unpack_type buf;
				Unpack(reinterpret_cast<char*>(&buf), sizeof(unpack_type), sizeof(unpack_type));
				return buf;
			}


	template <class buf_type, check_fuction istream_check>
		int _binary_istream<buf_type, istream_check>::GetReadPos(void) const
		{
			return m_ReadPos;
		}


	template <class buf_type, check_fuction istream_check>	
		void _binary_istream<buf_type, istream_check>::SetReadPos(int readPos)
		{
			do_check(istream_check, readPos >= 0 && readPos <= EndPos(), "can't set readPos after context bound!");
			m_ReadPos = readPos;
		}

	template <class buf_type, check_fuction istream_check>	
		void _binary_istream<buf_type, istream_check>::MoveReadPos(int offset)
		{
			int newPos = GetReadPos() + offset; // ʡ���˼ӷ����int��Χ�ļ��
			SetReadPos(newPos);
		}

	template <class buf_type, check_fuction istream_check>	
		int _binary_istream<buf_type, istream_check>::EndPos(void) const 
		{
			return static_cast<int>(m_ReadBuf.size());
		} 

	template <class buf_type, check_fuction istream_check>
		void _binary_istream<buf_type, istream_check>::UnpackString(::std::string& dstStr)
		{
			dstStr.clear();

			// �������(�����ƶ��α�����,��Ҫ��ԭ�α���鷳����)
			//
			{
				int dstStrLen = Read<int>(); // �����ַ�������
				int bufLeftLen = EndPos() - GetReadPos() - sizeof(int);
				do_check(istream_check, (dstStrLen >= 0) && (dstStrLen <= bufLeftLen), "distroyed string!");
			}

			// ��ʼȡ������,�����string
			try	{
				int dstStrLen = Unpack<int>();
				dstStr.reserve(dstStrLen);
				for (int i = 0; i < dstStrLen; i++)
					dstStr.append(1, Unpack<char>()); 
			}
			catch (bin_stream_exception&) {
				assert(false); // ǰ���Ѿ���֤�˳����˵�,�����Ӧ�����쳣
			}
		}

	template <class buf_type, check_fuction istream_check>
		void _binary_istream<buf_type, istream_check>::Read(char* pBuf, int bufSizeInBytes, int readPos, int sizeToRead) const
		{
			assert(pBuf != NULL); // ���Ᵽ��ԭʼ��assert, ��Ϊ��������϶���д�������.
			do_check(istream_check, bufSizeInBytes >= 0, "");

			memset(pBuf, 0, bufSizeInBytes);

			do_check(istream_check, (readPos >= 0) && (readPos <= EndPos()), "readPos exceed the bound of buf");
			do_check(istream_check, (sizeToRead >= 0) && ((readPos + sizeToRead) <= EndPos()), "read exceed the bound of buf!");
			do_check(istream_check, sizeToRead <= bufSizeInBytes, "dst buf is smaller than cpySize");

			const char* pReadBuf = m_ReadBuf.c_str();
			memcpy(pBuf, &pReadBuf[readPos], sizeToRead);
		}

	template <class buf_type, check_fuction istream_check>
		template <class read_type>
		read_type _binary_istream<buf_type, istream_check>::Read(int readPos) const
		{
			// �����������ֻ��ԭ������(int,char,DWORD��)���ܵ���Read�ӿ�.
			//BOOST_MPL_ASSERT_MSG(boost::is_fundamental<read_type>::value, obj_type_is_not_original_type, (read_type));

			read_type buf;
			Read(reinterpret_cast<char*>(&buf), sizeof(read_type), readPos, sizeof(read_type));
			return buf;
		}

END_ES_NAMESPACE








/*
  Copyright (C) 2005, 2006, 2007, 2008, 2011, 2012 Tadashi Watanabe <wac@umiushi.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef SKK_SIMPLE_STRING_HPP
#define SKK_SIMPLE_STRING_HPP

#include "skk_architecture.hpp"

namespace YaSkkServ
{
/// ñ���ʸ���󥯥饹�Ǥ���
/**
 * ���󥹥ȥ饯���ǻ��ꤷ���Хåե���ʸ������ɲä�����Ǵ������ޤ���
 *
 * �Хåե��ϰʲ��Τ褦�ʹ�¤�ˤʤäƤ��ޤ���
 *
 *<pre>
+--------+----------------+--------+
| MARGIN | ʸ����Хåե� | MARGIN |
+--------+----------------+--------+</pre>
 *
 * MARGIN �ϥǥХå��ʤɤ˻��Ѥ����ΰ�Ǥ������Τ��᰷�����ȤΤǤ���ʸ
 * ����Ĺ�ϡ����󥹥ȥ饯���ǻ��ꤷ���Хåե�����ʬ�����ʤ�ΤȤʤ��
 * ����
 *
 * <h1>Append �� Overwrite</h1>
 *
 * Append ��ʸ�����������ɲä��� \\0 �ǽ�ü���ޤ��� Overwrite �ϻ����
 * �֤�ʸ������񤭤�������Ǥ������ΤȤ� \\0 ���񤭤��뤳�ȤϤ���
 * �ޤ��󡣤Ĥޤ� \\0 �ǽ�ü����ޤ���
 *
 * �ޤȤ��Ȱʲ��Τ褦�ˤʤ�ޤ���
 *
 * <dl>
 * <dt>Append
 * <dd>ʸ������ɲá� \\0 ���ɲá�
 * <dt>Overwrite
 * <dd>ʸ������񤭡� \\0 �˿���ʤ���
 * </dl>
 *
 *
 * <h1>ʸ����Хåե� �� ������ MARGIN</h1>
 *
 * �̾�ʸ����ü�� \\0 ��ʸ����Хåե�����֤���ޤ���
 * DEBUG_PARANOIA_ASSERT() �Ǥ�ʸ����Хåե���˼��ޤ�ʤ���祢������
 * ���ޤ������ξ�ǡ������� MARGIN ����Ƭ 1 �Х��Ȥ� 0 �Ǥ��뤳�Ȥ��ݾ�
 * ���ޤ���
 *
 * �Ĥޤꡢ�̾�Ͻ�ü�Ǥ��� \\0 ��ʸ����Хåե����֤���ʤ���Фʤ��
 * ���󤬡������� \\0 ��ʸ����Хåե��˼��ޤ꤭��ʤ��ä���� (�̾��
 * ���ꤨ�ޤ���) �Ǥ������ MARGIN ����Ƭ�� 0 �Ǥ��뤳�Ȥ��顢ɬ��ʸ
 * ����Ͻ�ü����뤳�Ȥ��ݾڤ���ޤ���
 *
 *
 * <h1>���󥹥ȥ饯��</h1>
 *
 * �Хåե��ؤΥݥ��󥿤��Ϥ���硢�ݥ��󥿤� 0 �ΤȤ��������Ȥ�ͭ����
 * ��Х������Ȥ��ޤ���
 *
 *
 * <h1>����¾�᥽�åɤδ���ư��</h1>
 *
 * �����ʤɤ������ʾ�硢�������Ȥ�ͭ���ʤ�Х������Ȥ��ޤ���
 */
class SkkSimpleString
{
        SkkSimpleString(SkkSimpleString &source);
        SkkSimpleString& operator=(SkkSimpleString &source);

public:
        enum
        {
                MARGIN_SIZE = 4,
                STRING_SIZE_MAXIMUM = 8 * 1024
        };

private:
        void update_string()
        {
                string_size_cache_ = 0;
        }

        void fill_margin_for_debug()
        {
#ifdef YASKKSERV_DEBUG
                *(buffer_ + 0) = 0;
                *(buffer_ + buffer_size_ - MARGIN_SIZE + 0) = 0;
                for (int i = 1; i != MARGIN_SIZE; ++i)
                {
                        *(buffer_ + i) = static_cast<char>(1 + i);
                        *(buffer_ + buffer_size_ - MARGIN_SIZE + i) = static_cast<char>(1 + i);
                }
#endif  // YASKKSERV_DEBUG
        }

        void check_margin_for_debug() const
        {
#ifdef YASKKSERV_DEBUG
                DEBUG_ASSERT(*(buffer_ + 0) == 0);
                DEBUG_ASSERT(*(buffer_ + buffer_size_ - MARGIN_SIZE + 0) == 0);
                for (int i = 1; i != MARGIN_SIZE; ++i)
                {
                        DEBUG_ASSERT(*(buffer_ + i) == 1 + i);
                        DEBUG_ASSERT(*(buffer_ + buffer_size_ - MARGIN_SIZE + i) == 1 + i);
                }
#endif  // YASKKSERV_DEBUG
        }

        char *get_buffer() const
        {
                return buffer_ + MARGIN_SIZE;
        }

        void initialize_buffer()
        {
                fill_margin_for_debug();

                // ʸ������Ƭ�� 0 �ˡ�
                *get_buffer() = '\0';
                // ��ޡ��������Ƭ�� 0 �ˤ���ʸ�����ΰ褬���դξ���
                // ��ü����뤳�Ȥ��ݾڤ��ޤ���
                *(buffer_ + buffer_size_ - MARGIN_SIZE) = '\0';
        }

        static int compare_internal(const void *s_1, const void *s_2)
        {
                DEBUG_ASSERT_POINTER(s_1);
                DEBUG_ASSERT_POINTER(s_2);
                for (int i = 0;; ++i)
                {
                        int tmp_1 = *(static_cast<const char*>(s_1) + i);
                        int tmp_2 = *(static_cast<const char*>(s_2) + i);
                        if (tmp_1 == '\0')
                        {
                                if (tmp_2 == '\0')
                                {
                                        return 0;
                                }
                                else
                                {
                                        return -(i + 1);
                                }
                        }
                        else if (tmp_2 == '\0')
                        {
                                if (tmp_1 == '\0')
                                {
                                        return 0;
                                }
                                else
                                {
                                        return i + 1;
                                }
                        }
                        else if (tmp_1 > tmp_2)
                        {
                                return i + 1;
                        }
                        else if (tmp_1 < tmp_2)
                        {
                                return -(i + 1);
                        }
                }
                return 0;
        }

        static int compare_internal(const void *s_1, const void *s_2, int compare_size)
        {
                DEBUG_ASSERT_POINTER(s_1);
                DEBUG_ASSERT_POINTER(s_2);
                for (int i = 0; i != compare_size; ++i)
                {
                        int tmp_1 = *(static_cast<const char*>(s_1) + i);
                        int tmp_2 = *(static_cast<const char*>(s_2) + i);
                        if (tmp_1 == '\0')
                        {
                                if (tmp_2 == '\0')
                                {
                                        return 0;
                                }
                                else
                                {
                                        return -(i + 1);
                                }
                        }
                        else if (tmp_2 == '\0')
                        {
                                if (tmp_1 == '\0')
                                {
                                        return 0;
                                }
                                else
                                {
                                        return i + 1;
                                }
                        }
                        else if (tmp_1 > tmp_2)
                        {
                                return i + 1;
                        }
                        else if (tmp_1 < tmp_2)
                        {
                                return -(i + 1);
                        }
                }
                return 0;
        }

public:
        enum Flag
        {
                FLAG_RIGHT,
                FLAG_RIGHT_ZERO,
                FLAG_LEFT,
        };

        virtual ~SkkSimpleString()
        {
                check_margin_for_debug();
                DEBUG_ASSERT_POINTER(buffer_);
                DEBUG_ASSERT(buffer_size_ != 0);
                if (dynamic_allocation_flag_)
                {
                        delete[] buffer_;
                }
        }

        SkkSimpleString(int buffer_size) :
                buffer_(new char[buffer_size]),
                current_(get_buffer()),
                buffer_size_(buffer_size),
                string_size_cache_(0),
                dynamic_allocation_flag_(true)
        {
                DEBUG_ASSERT(buffer_size_ != 0);
                initialize_buffer();
        }

        SkkSimpleString(void *buffer,
                        int buffer_size) :
                buffer_(static_cast<char*>(buffer)),
                current_(get_buffer()),
                buffer_size_(buffer_size),
                string_size_cache_(0),
                dynamic_allocation_flag_(false)
        {
                DEBUG_ASSERT(buffer_size_ != 0);
                if (buffer_ == 0)
                {
                        DEBUG_ASSERT(0);
                }
                else
                {
                        initialize_buffer();
                }
        }

        template<size_t N> SkkSimpleString(char (&array)[N]) :
                buffer_(array),
                current_(get_buffer()),
                buffer_size_(N),
                string_size_cache_(0),
                dynamic_allocation_flag_(false)
        {
                DEBUG_ASSERT(buffer_size_ != 0);
                if (buffer_ == 0)
                {
                        DEBUG_ASSERT(0);
                }
                else
                {
                        fill_margin_for_debug();
                        *(buffer_ + 0) = '\0';
                        *(buffer_ + buffer_size_ - MARGIN_SIZE) = '\0';
                }
        }

/// ���� append() �ϥ᥽�åɤΥ���ǥå��������ꤷ�ޤ���
/**
 * \attention
 * �Хåե���Ȥ��ޤ魯���� setAppendIndex(0) �Ȥ����� reset() ���Ѥ�
 * �Ƥ���������
 *
 * \attention
 * ������ index ����ꤷ�����ϲ��⤻����ȴ���ޤ����ǥХå��ӥ�ɤǤ�
 * �������Ȥ��ޤ���
 */
        void setAppendIndex(int index)
        {
                if ((index < 0) || (index >= buffer_size_ - MARGIN_SIZE * 2 - 1))
                {
                        DEBUG_ASSERT(0);
                }
                else
                {
                        current_ = get_buffer() + index;
                }
        }

/// ���֤�ꥻ�åȤ���ʸ��������ꤷ�ޤ���
/**
 * �Хåե���Ȥ��ޤ路��ʣ����ʸ������ۤ��뤿��˻Ȥ��ޤ���
 *
 * \verbatim
ex.)
    char buffer[256];
    SimpleString object(buffer);

    object.append("ABC=");
    object.append(999);
    PRINT(object.getBuffer());

    object.reset();
    object.append("DEF=");
    object.append(999);
    PRINT(object.getBuffer());
\endverbatim
 */
        void reset()
        {
                current_ = get_buffer();
                *current_ = '\0';
        }

/// ʸ����Хåե��ؤΥݥ��󥿤��֤��ޤ���
/**
 * ��������ʸ����ϡ��ܥ᥽�åɤǼ�������ɬ�פ�����ޤ���
 */
        const char *getBuffer() const
        {
                return get_buffer();
        }

/// ������ʸ����Хåե��ؤΥݥ��󥿤��֤��ޤ���
/**
 * ����� append() �Ϥ��ΰ��֤ؼ¹Ԥ���ޤ���
 */
        const char *getCurrentBuffer() const
        {
                return current_;
        }

/// ʸ����Ĺ���֤��ޤ���ʸ����Ĺ�˽�ü�� \\0 �ϴޤߤޤ���
/**
 * \attention
 * ��Ｑ�˵���ʥǡ����ξ�硢����Ƿ�¬������ 0 ���֤��ޤ���
 */
        int getSize()
        {
                if (string_size_cache_)
                {
                        return string_size_cache_;
                }
                else
                {
                        ptrdiff_t diff = current_ - get_buffer();
                        string_size_cache_ = static_cast<int>(diff) + getSize(current_);
                        DEBUG_ASSERT(getSize(get_buffer()) == string_size_cache_);
                        return string_size_cache_;
                }
        }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
        int getSize(int limit)
        {
                if (string_size_cache_)
                {
                        return string_size_cache_;
                }
                else
                {
                        ptrdiff_t diff = current_ - get_buffer();
                        string_size_cache_ = getSize(current_, static_cast<int>(diff));
                        string_size_cache_ += static_cast<int>(diff);
                        DEBUG_ASSERT(getSize(get_buffer(), limit) == string_size_cache_);
                        return string_size_cache_;
                }
        }
#pragma GCC diagnostic pop

/// ���ꤷ���ͤ�ʸ����Хåե��ν�üľ���ޤ����ޤ���
/**
 * \attention
 * ʸ����Хåե��ν�ü�ؤϾ�� \\0 ���񤭹��ޤ�ޤ���
 */
        void fillStringBuffer(int c = '\0')
        {
                char *p = get_buffer();
                for (int i = 0; i != buffer_size_ - MARGIN_SIZE * 2 - 1; ++i)
                {
                        *p++ = static_cast<char>(c);
                }
                *p = '\0';
                update_string();
        }

/// size �Х��ȤΥǡ����� Append() ��ǽ�ʤ�п����֤��ޤ���
        bool isAppendSize(int size)
        {
                const int terminator_size = 1;
                int remain_size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                if (remain_size > size)
                {
                        return true;
                }
                else
                {
                        return false;
                }
        }

/// ������֤�ʸ�����񤭤��ޤ���
/**
 * \attention
 * c �� \\0 ����ꤷ�ƽ񤭹��ߡ�ʸ��������Ǥ��뤳�Ȥ��Ǥ��ޤ��� index
 * �����ߤ�ʸ������˼��ޤ�ʤ����ϲ��⤷�ޤ��󡣥������Ȥ�ͭ���ʤ��
 * �������Ȥ��ޤ���
 */
        void overwrite(char c, int index)
        {
                char *d = get_buffer() + index;
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - d - terminator_size);
                DEBUG_ASSERT_RANGE(size, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                if (index <= getSize())
                {
                        DEBUG_ASSERT(0);
                }
                else
                {
                        overwrite(c,
                                  d);
                        update_string();
                }
        }

/// ������֤�ʸ������񤭤��ޤ���
/**
 * \attention
 * ����ʸ���� p ¦�� \\0 �ϥ��ԡ�����ޤ���
 */
        void overwrite(const void *p, int index)
        {
                char *d = get_buffer() + index;
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - d - terminator_size);
                DEBUG_ASSERT_RANGE(size, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
#ifdef YASKKSERV_DEBUG_PARANOIA
                int tmp = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - (d + getSize(p) - 1) - terminator_size);
                DEBUG_ASSERT_RANGE(tmp, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                overwrite(p, d, size);
                update_string();
        }

/// ������֤�ʸ������񤭤��ޤ���ʸ���� p �� copy_size �Х���ʬ���ޤ��� \\0 ���դ���ޤ��ɲä���ޤ���
/**
 * \attention
 * ����ʸ���� p ¦�� \\0 �ϥ��ԡ�����ޤ���
 *
 * �̾�� overwrite() �Ȱۤʤꡢ p �� \\0 �ǽ�ü����Ƥ���ɬ�פ�����ޤ���
 */
        void overwrite(const void *p, int index, int copy_size)
        {
                char *d = get_buffer() + index;
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - d - terminator_size);
                DEBUG_ASSERT_RANGE(size, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
#ifdef YASKKSERV_DEBUG_PARANOIA
                int debug_size = copy_size;
                int tmp = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - (d + debug_size - 1) - terminator_size);
                DEBUG_ASSERT_RANGE(tmp, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                copy_size = (copy_size < size) ? copy_size : size;
                overwrite(p, d, size);
                update_string();
        }

/// ʸ������ɲä��ޤ���
/**
 * \attention
 * ʸ����� \\0 �ǽ�ü����ޤ���
 */
        void append(const SkkSimpleString &object)
        {
                append(object.getBuffer());
        }

/// ʸ������ɲä��ޤ���
/**
 * \attention
 * ʸ����� \\0 �ǽ�ü����ޤ���
 */
        void append(const void *p)
        {
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                DEBUG_ASSERT((size >= 1) && (size < buffer_size_ - MARGIN_SIZE * 2));
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int append_terminator_legth = 1;
                int tmp = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - (current_ + getSize(p) + append_terminator_legth - 1) - terminator_size);
                DEBUG_ASSERT_RANGE(tmp, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
                DEBUG_ASSERT_RANGE(getSize(p), 0, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                current_ = overwrite(p, current_, size);
                *current_ = '\0';
                update_string();
        }

/// ʸ������ɲä��ޤ���ʸ���� p �� copy_size �Х���ʬ���ޤ��� \\0 ���դ���ޤ��ɲä���ޤ���
/**
 * \attention
 * �̾�� append() �Ȱۤʤꡢ p �� \\0 �ǽ�ü����Ƥ���ɬ�פ�����ޤ���
 */
        void append(const void *p, int copy_size)
        {
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                DEBUG_ASSERT((size >= 1) && (size < buffer_size_ - MARGIN_SIZE * 2));
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int append_terminator_legth = 1;
                int debug_size = copy_size;
                int tmp = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - (current_ + debug_size + append_terminator_legth - 1) - terminator_size);
                DEBUG_ASSERT_RANGE(tmp, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
                DEBUG_ASSERT_RANGE(debug_size, 0, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                copy_size = (copy_size < size) ? copy_size : size;
                current_ = overwrite(p, current_, copy_size);
                *current_ = '\0';
                update_string();
        }

/// ʸ�����ɲä��ޤ���
/**
 * \attention
 * ʸ����� \\0 �ǽ�ü����ޤ���
 */
        void append(char c)
        {
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                DEBUG_ASSERT((size >= 1) && (size < buffer_size_ - MARGIN_SIZE * 2));
                const int append_terminator_size = 1;
                const int tmp_size = 1;
                int tmp = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - (current_ + 1 + append_terminator_size - 1) - terminator_size);
                DEBUG_ASSERT_RANGE(tmp, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
                DEBUG_ASSERT_RANGE(tmp_size, 1, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                *current_++ = c;
                *current_ = '\0';
                update_string();
        }

/// scalar �� 10 ��ʸ������Ѵ������ɲä��ޤ���
/**
 * \attention
 * ʸ����� \\0 �ǽ�ü����ޤ���
 */
        void append(int scalar, Flag flag = FLAG_LEFT, int column = 11)
        {
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                const int append_terminator_size = 1;
                DEBUG_ASSERT_RANGE(size, 11 + append_terminator_size, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                current_ = overwrite(current_, scalar, flag, column);
                *current_ = '\0';
                update_string();
        }

        void append(float scalar, int decimal, Flag flag = FLAG_LEFT, int column = 11)
        {
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                const int append_terminator_size = 1;
                DEBUG_ASSERT_RANGE(size, 11 + append_terminator_size, buffer_size_ - MARGIN_SIZE * 2 - 1);
#endif  // YASKKSERV_DEBUG_PARANOIA
                current_ = overwrite(current_, scalar, decimal, flag, column);
                *current_ = '\0';
                update_string();
        }

/// scalar �� 16 ��ʸ������Ѵ������ɲä��ޤ���
/**
 * \attention
 * ʸ����� \\0 �ǽ�ü����ޤ���
 */
        void appendHexadecimal(int scalar, int column = 8)
        {
#ifdef YASKKSERV_DEBUG_PARANOIA
                const int terminator_size = 1;
                int size = static_cast<int>(buffer_ + buffer_size_ - MARGIN_SIZE - current_ - terminator_size);
                const int append_terminator_size = 1;
                DEBUG_ASSERT_RANGE(size, 8 + append_terminator_size, buffer_size_ - MARGIN_SIZE * 2);
#endif  // YASKKSERV_DEBUG_PARANOIA
                current_ = overwriteHexadecimal(current_, scalar, column);
                *current_ = '\0';
                update_string();
        }

/// strncmp(3) ��Ʊ�ͤ�ʸ�������Ӥ��ޤ���
/**
 * ��ʬ���ȤΥ��֥������Ȥ�ʸ���󤬰�������٤ơ���������� -1 �ʲ�����
 * ������������� 0 �������礭����� 1 �ʾ���������֤��ޤ���
 */
        int compare(SkkSimpleString &object)
        {
                return compare_internal(getBuffer(), object.getBuffer());
        }

        int compare(const char *p)
        {
                return compare_internal(getBuffer(), p);
        }

        int compare(const char *p, int size)
        {
                return compare_internal(getBuffer(), p, size);
        }

/// ʸ���� 1 ʸ���֤��ޤ���
/**
 * index ���ϰϳ��ʤ�� \\0 ���֤��ޤ���
 *
 * \attention
 * �ϰϳ����������������ʽ����Ȥ��ư������������Ȥ��ʤ����Ȥ���դ�ɬ��
 * �Ǥ���
 */
        char getCharacter(int index)
        {
                if ((index >= getSize()) || (index < 0))
                {
                        return '\0';
                }
                return *(getBuffer() + index);
        }

        template<int index> char getCharacter()
        {
                if (index == 0)
                {
                        return *getBuffer();
                }
                else
                {
                        return getCharacter(index);
                }
        }

/// ������ʸ���� c ��Ʊ���ʤ�й�����ʸ���������ޤ��� repeat_flag �����ʤ�о�郎��Ω����¤������ޤ����������ʸ�������֤��ޤ���
/**
 * c �� 0 �ξ�硢���ԥ����ɤ������ޤ������ԥ����ɤ� \\r �ޤ��� \\n
 * �Ȥʤ�ޤ��� DOS �β��ԥ����ɤʤɤ����̻뤷�ʤ��Τǡ��μ¤˲��ԥ���
 * �ɤ�������ˤ� repeat_flag �򿿤ˤ���ɬ�פ�����ޤ���
 */
        int chomp(char c = 0, bool repeat_flag = true)
        {
                int index = getSize() - 1;
                if (index < 0)
                {
                        return 0;
                }
                int result = 0;
                char *p = get_buffer();
                if (c == 0)
                {
                        if (repeat_flag)
                        {
                                while ((*(p + index) == '\r') || (*(p + index) == '\n'))
                                {
                                        *(p + index) = '\0';
                                        ++result;
                                        if (--index <= 0)
                                        {
                                                break;
                                        }
                                }
                        }
                        else
                        {
                                if ((*(p + index) == '\r') || (*(p + index) == '\n'))
                                {
                                        *(p + index) = '\0';
                                        ++result;
                                }
                        }
                }
                else
                {
                        if (repeat_flag)
                        {
                                while (*(p + index) == c)
                                {
                                        *(p + index) = '\0';
                                        ++result;
                                        if (--index <= 0)
                                        {
                                                break;
                                        }
                                }
                        }
                        else
                        {
                                if (*(p + index) == c)
                                {
                                        *(p + index) = '\0';
                                        ++result;
                                }
                        }
                }
                update_string();
                return result;
        }

/// ʸ����ΥХ��ȥ��������֤��ޤ����������˽�ü�� \\0 �ϴޤߤޤ���
/**
 * \attention
 * limit �ʾ�ε���ʥǡ����ξ�硢����Ƿ�¬������ 0 ���֤��ޤ�������
 * �Ȥ��������Ȥ�ͭ���ʤ�Х������Ȥ��ޤ���
 */
        static int getSize(const void *p, int limit = 64 * 1024)
        {
                const char *tmp = static_cast<const char*>(p);
                for (int i = 0; i != limit; ++i)
                {
                        if (*tmp++ == '\0')
                        {
                                return i;
                        }
                }
                DEBUG_ASSERT(0);
                return 0;
        }

/// ʸ���򥳥ԡ����ޤ���
/**
 * \attention
 * c �� \\0 ����ꤷ�ƽ񤭹��ߡ�ʸ��������Ǥ��뤳�Ȥ��Ǥ��ޤ���
 */
        static void overwrite(char c, void *destination)
        {
                char *p = static_cast<char*>(destination);
                *p = c;
        }

/// ʸ����򥳥ԡ����ޤ���ʸ����κǸ� + 1 ��ؤ��ݥ��󥿤��֤��ޤ���
/**
 * \attention
 * \\0 �ϥ��ԡ�����ޤ���
 */
        static char *overwrite(const void *source, void *destination, int size)
        {
                const char *s = static_cast<const char*>(source);
                char *d = static_cast<char*>(destination);
                for (; size > 0; --size)
                {
                        if (*s == '\0')
                        {
                                break;
                        }
                        *d++ = *s++;
                }
                return d;
        }

/// column ��� 10 ��ʸ������������ޤ���ʸ����κǸ� + 1 ��ؤ��ݥ��󥿤��֤��ޤ���
/**
 * ���� column �˴ޤޤ�ޤ���
 *
 * \attention
 * column ���¿���η��ɽ������ޤ��󡣤��ξ�硢��̤η�Ϻ���ޤ���
 */
        static char *overwrite(void *destination, int scalar, Flag flag = FLAG_LEFT, int column = 11)
        {
                static const int table[] =
                {
                        1,
                        10,
                        100,
                        1000,
                        10000,

                        100000,
                        1000000,
                        10000000,
                        100000000,
                        1000000000,
                };
                int space = (flag == FLAG_RIGHT_ZERO) ? '0' : ' ';
                int sign;
                int absolute;
                const int table_size = sizeof(table)/sizeof(table[0]);
                char *p = static_cast<char*>(destination);
                bool zero_print_flag = false;
                bool right_flag = ((flag == FLAG_RIGHT_ZERO) || (flag == FLAG_RIGHT)) ? true : false;
                DEBUG_ASSERT_RANGE(column, 1, 11);
                if (scalar < 0)
                {
                        sign = '-';
                        absolute = -scalar;
                }
                else
                {
                        sign = 0;
                        absolute = scalar;
                        if (right_flag && (column == 11))
                        {
                                *p++ = ' ';
                        }
                }
                for (int i = table_size - 1; i >= 0; --i)
                {
                        int d = absolute / table[i];
                        if (d == 0)
                        {
                                if (zero_print_flag || (i == 0))
                                {
                                        if (i < column)
                                        {
                                                *p++ = '0';
                                        }
                                }
                                else
                                {
                                        if (right_flag)
                                        {
                                                if (i < column)
                                                {
                                                        *p++ = static_cast<char>(space);
                                                }
                                        }
                                }
                        }
                        else
                        {
                                if (sign != 0)
                                {
                                        if (i < column - 1)
                                        {
                                                if (column != 11)
                                                {
                                                        --column;
                                                        if (p != destination)
                                                        {
                                                                --p;
                                                        }
                                                }
                                                *p++ = static_cast<char>(sign);
                                        }
                                        sign = 0;
                                }
                                absolute -= d * table[i];
                                zero_print_flag = true;
                                if (i < column)
                                {
                                        *p++ = static_cast<char>('0' + d);
                                }
                        }
                }
                return p;
        }

/// column ��� 10 ��ʸ������������ޤ���ʸ����κǸ� + 1 ��ؤ��ݥ��󥿤��֤��ޤ���
/**
 * ���ȥɥåȤ� column �˴ޤޤ�ޤ���
 *
 * \attention
 * column ���¿���η��ɽ������ޤ��󡣤��ξ�硢��̤η�Ϻ���ޤ���
 */
        static char *overwrite(void *destination, float scalar, int decimal = 1, Flag flag = FLAG_LEFT, int column = 11)
        {
                DEBUG_ASSERT_RANGE(column, 2, 11);
                DEBUG_ASSERT_RANGE(column - 1 - decimal, 1, 9);
                char *p = static_cast<char*>(destination);
                p = overwrite(p, static_cast<int>(scalar), flag, column - 1 - decimal);
                *p++ = '.';
                int po = 1;
                for (int i = 0; i != decimal; ++i)
                {
                        po *= 10;
                }
                p = overwrite(p, static_cast<int>(scalar * static_cast<float>(po)) % po, FLAG_RIGHT_ZERO, decimal);
                return p;
        }

/// column ��� 16 ��ʸ������������ޤ���ʸ����κǸ� + 1 ��ؤ��ݥ��󥿤��֤��ޤ���
/**
 * \attention
 * column ���¿���η��ɽ������ޤ��󡣤��ξ�硢��̤η�Ϻ���ޤ���
 */
        static char *overwriteHexadecimal(void *destination, int scalar, int column = 8)
        {
                char *p = static_cast<char*>(destination);
                DEBUG_ASSERT_RANGE(column, 1, 8);
                for (int i = column - 1; i >= 0; --i)
                {
                        int tmp = (scalar >> (i * 4)) & 0xf;
                        if (tmp > 0xa)
                        {
                                *p++ = static_cast<char>('a' - 0xa + tmp);
                        }
                        else
                        {
                                *p++ = static_cast<char>('0' + tmp);
                        }
                }
                return p;
        }

/// strncmp(3) ��Ʊ�ͤ�ʸ�������Ӥ��ޤ���
/**
 * s_1 �� s_2 ����٤ơ���������� -1 �ʲ������������������ 0 ��������
 * ������� 1 �ʾ���������֤��ޤ���
 */
        static int compare(const void *s_1, const void *s_2)
        {
                return compare_internal(s_1, s_2);
        }

        static int compare(const void *s_1, const void *s_2, int buffer_size)
        {
                return compare_internal(s_1, s_2, buffer_size);
        }

/// base_size �Х��Ȥ�ʸ���� base �� search_size �Х��Ȥ� search �ˤ�곫�Ϥ���Ƥ���п����֤��ޤ���
/**
 * ��ü�����ɤȤ��� ' ' �ޤ��� '\0' ��ǧ�����ޤ���
 */
        static bool startWith(const char *base, const char *search, int base_size, int search_size)
        {
                DEBUG_ASSERT_POINTER(base);
                DEBUG_ASSERT_POINTER(search);
                DEBUG_ASSERT(base_size > 0);
                DEBUG_ASSERT(search_size > 0);
                for (int i = 0; ; ++i)
                {
                        if (i >= search_size)
                        {
                                return true;
                        }
                        int s_2 = *(reinterpret_cast<const unsigned char*>(search + i));
                        if ((s_2 == ' ') || (s_2 == '\0'))
                        {
                                return true;
                        }
                        if (i >= base_size)
                        {
                                return false;
                        }
                        int s_1 = *(reinterpret_cast<const unsigned char*>(base + i));
                        if (s_1 != s_2)
                        {
                                return false;
                        }
                        if ((s_1 == ' ') || (s_1 == '\0'))
                        {
                                return false;
                        }
                }
                return false;   // NOTREACHED
        }

/// base_size �Х��Ȥ�ʸ���� base �� c ���ޤޤ�Ƥ���п����֤��ޤ���
/**
 * ��ü�����ɤȤ��� ' ' �ޤ��� '\0' ��ǧ�����ޤ���
 */
        static bool search(const char *base, char c, int base_size)
        {
                DEBUG_ASSERT_POINTER(base);
                DEBUG_ASSERT(base_size > 0);
                for (int i = 0; ; ++i)
                {
                        if (i >= base_size)
                        {
                                return false;
                        }
                        int s_1 = *(reinterpret_cast<const unsigned char*>(base + i));
                        if (s_1 == c)
                        {
                                return true;
                        }
                        if ((s_1 == ' ') || (s_1 == '\0'))
                        {
                                return false;
                        }
                }
                return false;   // NOTREACHED
        }

private:
        char *buffer_;
        char *current_;
        int buffer_size_;
        int string_size_cache_;
        bool dynamic_allocation_flag_;
};
}

#endif // SKK_SIMPLE_STRING_HPP

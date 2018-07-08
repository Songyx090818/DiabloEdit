#pragma once

#include <vector>

// bits
template<typename T>
class Bits
{
	T & v_;
	int b_;
public:
	Bits(T & v, int b) :v_(v), b_(b) {}
	T & value() const { return v_; }
	int bits() const { return b_; }
};

template<typename T>
Bits<T> bits(T & v, int b) { return Bits<T>(v, b); }

template<typename T>
Bits<const T> bits(const T & v, int b) { return Bits<const T>(v, b); }

// offset_value
template<typename T>
class OffsetValue
{
	const T & v_;
	DWORD off_;
public:
	OffsetValue(const T & v, DWORD off) :v_(v), off_(off) {}
	T value() const { return v_; }
	DWORD offset() const { return off_; }
};

template<typename T>
OffsetValue<T> offset_value(const T & value, DWORD offset) {
	return OffsetValue<T>(value, offset);
}

//��from����len�������ݵ�to���ֱ�����from��ǰfromOff���أ���to��ǰtoOff���ء�
void CopyBits(const BYTE * from, BYTE * to, DWORD fromOff, DWORD toOff, DWORD len);

class CInBitsStream
{
	std::vector<BYTE> data_;
	DWORD bytes_, bits_;
	bool bad_;
public:
	CInBitsStream()
		: bytes_(0)
		, bits_(0)
		, bad_(false){}
	DWORD BytePos() const{return bytes_;}
	bool Good() const { return !bad_; }
	//�����λ
	void SeekBack(DWORD back) {
		if (ensurePos(bytes_ - back))
			bytes_ -= back;
	}
	void ReadFile(CFile & cf){
		data_.resize(size_t(cf.GetLength()));
		cf.Read(&data_[0],UINT(cf.GetLength()));
	}
	//�ֽڶ�ȡ
	CInBitsStream & operator >>(DWORD & value) {return readPod(value);}
	CInBitsStream & operator >>(WORD & value){ return readPod(value); }
	CInBitsStream & operator >>(BYTE & value){ return readPod(value); }
	template<typename T, int N>
	CInBitsStream & operator >>(T (&value)[N]){
		for(T & v : value)
			operator >>(v);
		return *this;
	}
	template<int N>
	CInBitsStream & operator >>(BYTE (&value)[N]){
		if (ensure(N)) {
			::CopyMemory(value, &data_[bytes_], N);
			bytes_ += N;
		}
		return *this;
	}
	//λ��ȡ
	void AlignByte(){	//����8λ���ж��룬���������λ
		bytes_ += (bits_ + 7) / 8;
		bits_ = 0;
	}
	CInBitsStream & operator >>(BOOL & b) {return readBits(bits(b, 1));}
	CInBitsStream & operator >>(const Bits<DWORD> & m) { return readBits(m); }
	CInBitsStream & operator >>(const Bits<WORD> & m) { return readBits(m); }
	CInBitsStream & operator >>(const Bits<BYTE> & m) {return readBits(m);}
	void ReadBit(BOOL & value) { operator >>(value); }
	void ReadBits(DWORD & value, int len) { operator >>(bits(value, len)); }
	void ReadBits(WORD & value, int len) { operator>>(bits(value, len)); }
	void ReadBits(BYTE & value, int len) { operator>>(bits(value, len)); }
	//vector<BYTE>
	CInBitsStream & operator >>(std::vector<BYTE> & vec){
		if (ensure(0))
			vec.assign(data_.begin() + bytes_, data_.end());
		return *this;
	}
	//�Ѳ���ʶ�������ת��
	void Restore(std::vector<BYTE> & vec,DWORD from,DWORD to){
		ASSERT(to >= from);
		if(ensurePos(from) && ensurePos(to))
			vec.assign(&data_[from], &data_[to]);
	}
	//�����ã��õ�������2����λ��
	CString ToString(DWORD len = 32) const;
private:
	template<typename T>
	CInBitsStream & readPod(T & value) {
		if (ensure(sizeof value)) {
			value = *reinterpret_cast<T *>(&data_[bytes_]);
			bytes_ += sizeof value;
		}
		return *this;
	}
	template<typename T>
	CInBitsStream & readBits(const Bits<T> & m) {
		if (ensure(0, m.bits(), sizeof(T) * 8)) {
			m.value() = 0;
			BYTE * to = reinterpret_cast<BYTE *>(&m.value());
			::CopyBits(&data_[bytes_], to, bits_, 0, m.bits());
			bytes_ += (bits_ + m.bits()) / 8;
			bits_ = (bits_ + m.bits()) % 8;
		}
		return *this;
	}
	bool ensure(DWORD bytes, DWORD bits = 0, DWORD maxBits = 0) {
		bad_ = (bad_
			|| !(bits > 0 ? bits <= maxBits : bits_ == 0)
			|| !(bytes_ + bytes + (bits_ + bits + 7) / 8 <= data_.size()));
		return !bad_;
	}
	bool ensurePos(DWORD pos) {
		bad_ = (bad_ || pos > data_.size());
		return !bad_;
	}
};

class COutBitsStream
{
	std::vector<BYTE> data_;
	DWORD bytes_, bits_;
	bool bad_;
public:
	COutBitsStream() :bytes_(0), bits_(0), bad_(false) {}
	DWORD BytePos() const { return bytes_; }
	bool good() const { return !bad_; }
	/*void WriteFile(CFile & cf){
	ASSERT(data_.size() && _T("CBinDataStream::WriteFile(CFile & cf)"));
	cf.Write(&data_[0],UINT(data_.size()));
	cf.Flush();
}*/
	//�ֽڶ�ȡ
	COutBitsStream & operator <<(DWORD value) { return writePod(value); }
	COutBitsStream & operator <<(WORD value) { return writePod(value); }
	COutBitsStream & operator <<(BYTE value) { return writePod(value); }
	template<typename T, int N>
	COutBitsStream & operator <<(const T (&value)[N]) {
		for (auto & v : value)
			operator <<(v);
		return *this;
	}
	template<int N>
	COutBitsStream & operator <<(BYTE (&value)[N]) {
		ensure(N);
		::CopyMemory(&data_[bytes_], value, N);
		bytes_ += N;
		return *this;
	}
	template<typename T>
	COutBitsStream & operator <<(const OffsetValue<T> & m) {
		if (ensurePos(m.offset())) {
			const auto save = bytes_;
			bytes_ = m.offset();
			operator <<(m.value());
			bytes_ = save;
		}
		return *this;
	}
	//λ��ȡ
	COutBitsStream & operator <<(const Bits<const DWORD> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<const WORD> & m) { return writeBits(m); }
	COutBitsStream & operator <<(const Bits<const BYTE> & m) { return writeBits(m); }

private:
	template<typename T>
	COutBitsStream & writePod(T v) {
		if (ensure(sizeof v)) {
			*reinterpret_cast<T *>(&data_[bytes_]) = v;
			bytes_ += sizeof v;
		}
		return *this;
	}
	template<typename T>
	COutBitsStream & writeBits(const Bits<const T> & m) {
		if (ensure(0, m.bits(), sizeof(T) * 8)) {
			const BYTE * from = reinterpret_cast<const BYTE *>(&m.value());
			CopyBits(from, &data_[bytes_], 0, bits_, m.bits());
			bytes_ += (bits_ + m.bits()) / 8;
			bits_ = (bits_ + m.bits()) % 8;
		}
		return *this;
	}
	bool ensure(DWORD bytes, DWORD bits = 0, DWORD maxBits = 0) {
		bad_ = (bad_ || !(bits > 0 ? bits <= maxBits : bits_ == 0));
		if (!bad_) {
			const DWORD old = data_.size(), need = bytes + (bits_ + bits + 7) / 8;
			if (bytes_ + need > old)
				data_.resize(old + (old >> 1) + need);
		}
		return !bad_;
	}
	bool ensurePos(DWORD pos) {
		bad_ = (bad_ || pos > data_.size());
		return !bad_;
	}
};
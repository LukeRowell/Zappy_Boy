#pragma once
namespace bitwise {

	inline unsigned char compose_bits(const unsigned char high, const unsigned char low) {
		return static_cast<unsigned char>(high << 1 | low);
	}

	inline unsigned char compose_nibbles(const unsigned char high, const unsigned char low) {
		return static_cast<unsigned char>((high << 4) | low);
	}

	inline unsigned short compose_bytes(const unsigned char high, const unsigned char low) {
		return static_cast<unsigned short>((high << 8) | low);
	}

	inline bool check_bit(const unsigned char value, const unsigned char bit) {
		return (value & (1 << bit)) != 0;
	}

	inline unsigned char bit_value(const unsigned char value, const unsigned char bit) {
		return (value >> bit) & 1;
	}

	inline unsigned char set_bit(const unsigned char value, const unsigned char bit) {
		auto value_set = value | (1 << bit);
		return static_cast<unsigned char>(value_set);
	}

	inline unsigned char clear_bit(const unsigned char value, const unsigned char bit) {
		auto value_cleared = value & ~(1 << bit);
		return static_cast<unsigned char>(value_cleared);
	}

	inline unsigned char set_bit_to(const unsigned char value, const unsigned char bit, bool bit_on) {
		return bit_on
			? set_bit(value, bit)
			: clear_bit(value, bit);
	}

} // namespace bitwise
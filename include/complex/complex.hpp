#ifndef COMPLEX_HPP_PENC
#define COMPLEX_HPP_PENC

#include <string>
#include <sstream>
#include <type_traits>

namespace penci {
	template<typename Derived>
	class ComplexBase;

	template<typename L, typename R>
	class Add;

	namespace detail {
		template<typename T>
		struct Traits {
			using Scalar							= T;
			static constexpr inline bool IsScalar	= true;
			static constexpr inline bool IsFunction = false;
		};

		template<typename T>
		struct Traits<ComplexBase<T>> {
			using Scalar							= typename Traits<T>::Scalar;
			static constexpr inline bool IsScalar	= false;
			static constexpr inline bool IsFunction = false;
		};

		template<typename L, typename R>
		struct Traits<Add<L, R>> {
			using Scalar							= typename Traits<L>::Scalar;
			static constexpr inline bool IsScalar	= false;
			static constexpr inline bool IsFunction = true;
		};

		template<typename LHS, typename RHS>
		class Add {
		public:
			using Scalar							   = typename Traits<LHS>::Scalar;
			static constexpr inline bool LeftIsScalar  = Traits<LHS>::IsScalar;
			static constexpr inline bool RightIsScalar = Traits<RHS>::IsScalar;

			inline Scalar re(const LHS &lhs, const RHS &rhs) const {
				if constexpr (LeftIsScalar && !RightIsScalar) return lhs + rhs.re();
				if constexpr (!LeftIsScalar && RightIsScalar) return lhs.re() + rhs;
				if constexpr (!LeftIsScalar && !RightIsScalar) return lhs.re() + rhs.re();
			}

			inline Scalar im(const LHS &lhs, const RHS &rhs) const {
				if constexpr (LeftIsScalar && !RightIsScalar) return rhs.im();
				if constexpr (!LeftIsScalar && RightIsScalar) return lhs.im();
				if constexpr (!LeftIsScalar && !RightIsScalar) return lhs.im() + rhs.im();
			}
		};

		template<typename FUNC, typename LHS, typename RHS>
		class BinOp : public ComplexBase<BinOp<FUNC, LHS, RHS>> {
		public:
			using Scalar							   = typename Traits<LHS>::Scalar;
			static constexpr inline bool LeftIsScalar  = Traits<LHS>::IsScalar;
			static constexpr inline bool RightIsScalar = Traits<RHS>::IsScalar;

			BinOp() = delete;
			BinOp(const LHS &left, const RHS &right) : m_lhs(left), m_rhs(right), m_lambda() {}

			inline Scalar re() const { return m_lambda.re(m_lhs, m_rhs); }
			inline Scalar im() const { return m_lambda.im(m_lhs, m_rhs); }

		private:
			const FUNC &m_lambda;
			const LHS &m_lhs;
			const RHS &m_rhs;
		};
	} // namespace detail

	template<typename Derived>
	class ComplexBase {
	public:
		using Scalar = typename detail::Traits<Derived>::Scalar;
		using This	 = ComplexBase<Derived>;

		ComplexBase() : m_re(), m_im() {}
		ComplexBase(const Scalar &re, const Scalar &im) : m_re(re), m_im(im) {}
		ComplexBase(const ComplexBase &other) : m_re(other.m_re), m_im(other.m_im) {}
		ComplexBase(const ComplexBase &&other) noexcept :
				m_re(std::move(other.m_re)), m_im(std::move(other.m_im)) {}

		template<typename T> // , typename std::enable_if_t<detail::Traits<T>::IsFunction, int> = 0>
		explicit ComplexBase(const T &other) : m_re(other.re()), m_im(other.im()) {}

		template<typename S>
		explicit ComplexBase(const ComplexBase<S> &other) : m_re(other.re()), m_im(other.im()) {}

		ComplexBase &operator=(const ComplexBase &other) {
			m_re = other.m_re;
			m_im = other.m_im;
			return derived();
		}

		template<typename S>
		ComplexBase &operator=(const ComplexBase<S> &other) {
			m_re = other.re();
			m_im = other.im();
		}

		template<typename D>
		inline auto operator+(const D &other) const {
			return detail::Add<This, D>(*this, other);
		}

		inline const Scalar &re() const { return m_re; }
		inline Scalar &re() { return m_re; }
		inline const Scalar &im() const { return m_im; }
		inline Scalar &im() { return m_im; }

		[[nodiscard]] std::string str() const {
			std::stringstream ss;
			ss << re();
			if (m_im == 0) return ss.str();

			if (m_im > 0)
				ss << " + " << im() << "j";
			else
				ss << " - " << -im() << "j";
			return ss.str();
		}

	private:
		[[nodiscard]] const inline Derived &derived() const {
			return *static_cast<Derived *>(this);
		}

		[[nodiscard]] inline Derived &derived() { return *static_cast<Derived *>(this); }

	private:
		Scalar m_re;
		Scalar m_im;
	};
} // namespace penci

#endif // COMPLEX_HPP_PENC
#pragma once

#include <arcsoft_face_sdk.h>
#include <merror.h>
#include <string>
#include <vector>
#include <atomic>
#include <optional>
#include <initializer_list>

namespace ohtoai
{

	// Defines error
	namespace error{
		template <typename T, T ...SuccessCodes>
		struct ErrorCode{
			ErrorCode() = default;
			ErrorCode(const ErrorCode&) = default;
			ErrorCode(ErrorCode&&) = default;
			ErrorCode& operator =(const ErrorCode&) = default;
			ErrorCode& operator =(ErrorCode&&) = default;
			ErrorCode(const T& t) : code_{ t } {}

			operator bool() const{
				return success();
			}
			bool success() const{
				for (auto s : SuccessCodeList)
				{
					if (s == code_)
						return true;
				}
				return false;
			}
			T code() const{
				return code_;
			}

		protected:
			inline static std::vector<T> SuccessCodeList{ SuccessCodes ... };
			T code_{ SuccessCodeList.front() };
		};
	}


	namespace arc
	{
		using ArcErrorCode = error::ErrorCode<std::uint32_t, MOK, MERR_ASF_ALREADY_ACTIVATED>;

		using Rect = MRECT;
		using Point = MPOINT;


		struct FaceFeature: std::vector<std::uint8_t> {
			using std::vector<std::uint8_t>::vector;
		};

		using OrientPriority = ASF_OrientPriority;
		using DetectMode = ASF_DetectMode;
		using CompareModel = ASF_CompareModel;

		/// <summary>
		/// ArcSoft Face Engine
		/// </summary>
		class ASFFaceEngine
		{

		public:

			std::optional<FaceFeature> faceFeatureExtractEx(LPASF_ImageData imageData, LPASF_SingleFaceInfo singleFaceInfo)
			{
				FaceFeature feature;
				return faceFeatureExtractEx(feature, imageData, singleFaceInfo)
					? std::make_optional(feature) : std::nullopt;
			}

			ArcErrorCode faceFeatureExtractEx(FaceFeature& feature, LPASF_ImageData imageData, LPASF_SingleFaceInfo singleFaceInfo)
			{
				::ASF_FaceFeature f_;
				auto ret = ::ASFFaceFeatureExtractEx(engineHandle_, imageData, singleFaceInfo, &f_);
				feature = FaceFeature(f_.feature, f_.feature + f_.featureSize);
				return ret;
			}

			std::optional<float> faceFeatureCompare(const FaceFeature& f1, const FaceFeature& f2, CompareModel model = CompareModel::ASF_LIFE_PHOTO)
			{
				float confidenceLevel{};
				return faceFeatureCompare(confidenceLevel, f1, f2, model)
					? std::make_optional(confidenceLevel) : std::nullopt;
			}

			ArcErrorCode faceFeatureCompare(float & confidenceLevel, const FaceFeature& f1, const FaceFeature& f2, CompareModel model = CompareModel::ASF_LIFE_PHOTO)
			{
				::ASF_FaceFeature af1{ const_cast<MByte*>(f1.data()), static_cast<MInt32>(f1.size()) };
				::ASF_FaceFeature af2{ const_cast<MByte*>(f2.data()), static_cast<MInt32>(f2.size()) };
				auto ret = ::ASFFaceFeatureCompare(engineHandle_, &af1, &af2, &confidenceLevel, model);
				return ret;
			}


		public:

			ArcErrorCode static onlineActivation(std::string appId, std::string sdkKey) {
				ArcErrorCode ret = ::ASFOnlineActivation(const_cast<MPChar>(appId.c_str()), const_cast<MPChar>(sdkKey.c_str()));
				onlineActived_ = ret;
				return  ret;
			}

			ArcErrorCode initEngine(DetectMode detectMode,
				OrientPriority	detectFaceOrientPriority,
				int				detectFaceScaleVal,
				int				detectFaceMaxNum,
				int				combinedMask)
			{
				auto ret = ::ASFInitEngine(detectMode, detectFaceOrientPriority
					, detectFaceScaleVal, detectFaceMaxNum
					, combinedMask, &engineHandle_);
				valid_ = (ret == MOK);
				return ret;
			}

			ArcErrorCode uninitEngine()
			{
				if (valid_) {
					valid_ = false;
					return ::ASFUninitEngine(engineHandle_);
				}
				return {};
			}


			virtual ~ASFFaceEngine()
			{
				uninitEngine();
			}
		public:

			[[deprecate]] ::MHandle engineHandle() { return engineHandle_; }

			bool valid() const
			{
				return valid_;
			}
			static bool onlineActived()
			{
				return onlineActived_;
			}

		protected:
			std::atomic_bool valid_{ false };
			inline static std::atomic_bool onlineActived_{ false };
		protected:
			::MHandle engineHandle_{};
		};
	}
}
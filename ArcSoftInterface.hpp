#pragma once

#include <arcsoft_face_sdk.h>
#include <merror.h>
#include <string>
#include <vector>
#include <atomic>
#include <optional>

namespace ohtoai
{
	namespace arc
	{
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
				::ASF_FaceFeature f_;
				auto ret = ::ASFFaceFeatureExtractEx(engineHandle_, imageData, singleFaceInfo, &f_);
				if (ret == MOK)
					return FaceFeature(f_.feature, f_.feature + f_.featureSize);
				else
					return {};
			}

			std::optional<float> faceFeatureCompare(const FaceFeature&f1, const FaceFeature&f2, CompareModel model= CompareModel::ASF_LIFE_PHOTO)
			{
				float confidenceLevel{};
				::ASF_FaceFeature af1{ const_cast<MByte*>(f1.data()), static_cast<MInt32>(f1.size()) };
				::ASF_FaceFeature af2{ const_cast<MByte*>(f2.data()), static_cast<MInt32>(f2.size()) };
				auto ret = ::ASFFaceFeatureCompare(engineHandle_, &af1, &af2, &confidenceLevel, model);
				if (ret == MOK)
					return confidenceLevel;
				else
					return {};
			}

			
		public:

			bool static onlineActivation(std::string appId, std::string sdkKey) {
				auto ret = ::ASFOnlineActivation(const_cast<MPChar>(appId.c_str()), const_cast<MPChar>(sdkKey.c_str()));
				return onlineActived_ = (ret == MOK || ret == MERR_ASF_ALREADY_ACTIVATED);
			}

			bool initEngine(DetectMode detectMode,
				OrientPriority	detectFaceOrientPriority,
				int				detectFaceScaleVal,
				int				detectFaceMaxNum,
				int				combinedMask)
			{
				auto ret = ::ASFInitEngine(detectMode, detectFaceOrientPriority
					, detectFaceScaleVal, detectFaceMaxNum
					, combinedMask, &engineHandle_);
				return valid_ = (ret == MOK);
			}

			bool uninitEngine()
			{
				if (valid_) {
					valid_ = false;
					auto ret = ::ASFUninitEngine(engineHandle_);
					return ret == MOK;
				}
				return true;
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
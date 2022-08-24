#pragma once

#include <arcsoft_face_sdk.h>
#include <merror.h>
#include <string>
#include <vector>
#include <atomic>
#include <optional>
#include <initializer_list>
#include <QMetaEnum>

namespace ohtoai
{
	// Type tool
	namespace type_tool {
		template <typename Base>
		struct CustomConvertClass : Base
		{
			CustomConvertClass() = default;
			CustomConvertClass(const CustomConvertClass&) = default;
			CustomConvertClass(CustomConvertClass&&) = default;
			CustomConvertClass& operator =(const CustomConvertClass&) = default;
			CustomConvertClass& operator =(CustomConvertClass&&) = default;
			CustomConvertClass(const Base& t) : Base{ t } {}
		};
	}

#define OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(_class) \
	_class() = default;							\
	_class(const _class&) = default;			\
	_class(_class&&) = default;					\
	_class& operator =(const _class&) = default;\
	_class& operator =(_class&&) = default;

	// Defines error
	namespace error{
		template <typename T, T ...SuccessCodes>
		struct ErrorCode{
			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(ErrorCode)
			ErrorCode(const T& t) : code_{ t } {}

			operator bool() const{
				return success();
			}

			operator T() const {
				return code_;
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
		using Rect = type_tool::CustomConvertClass<MRECT>;
		using Point = type_tool::CustomConvertClass<MPOINT>;

		enum ArcEngineMask {
			EngineMaskNone = 0x00000000,			//
			EngineMaskFaceDetect = 0x00000001,		//
			EngineMaskFaceRecognition = 0x00000004,	//
			EngineMaskAge = 0x00000008,				//
			EngineMaskGender = 0x00000010,			//
			EngineMaskFace3DAngle = 0x00000020,		//
			EngineMaskLiveness = 0x00000080,		//
			EngineMaskIRLiveness = 0x00000400,		//
		};
		using ArcEngineMasks = QFlags<ArcEngineMask>;
		Q_DECLARE_OPERATORS_FOR_FLAGS(ArcEngineMasks);

		enum DetectMode {
			DetectModeVideo = 0x00000000, // Video模式
			DetectModeImage = 0xFFFFFFFF, // Image模式
		};

		enum OrientPriority {
			OrientPriority0 = 1,		//
			OrientPriority90,			//
			OrientPriority270,			//
			OrientPriority180,			//
			OrientPriorityAll			//
		};

		// orientation
		enum OrientCode {
			OrientCode0 = 0x1,	// 0 degree
			OrientCode90,		// 90 degree
			OrientCode270,		// 270 degree
			OrientCode180,   	// 180 degree
			OrientCode30,		// 30 degree
			OrientCode60,		// 60 degree
			OrientCode120,		// 120 degree
			OrientCode150,		// 150 degree
			OrientCode210,		// 210 degree
			OrientCode240,		// 240 degree
			OrientCode300,		// 300 degree
			OrientCode330		// 330 degree
		};

		enum DetectModel {
			DetectModelRGB = 0x1	// RGB
		};

		enum CompareModel {
			CompareModelLifePhoto = 0x1,	//
			CompareModelIdPhoto = 0x2		//
		};

		struct ArcVersion{
			std::string Version;			//
			std::string BuildDate;			//
			std::string CopyRight;			//

			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(ArcVersion);
			ArcVersion(const ASF_VERSION v)
				: Version{ v.Version }
				, BuildDate{ v.BuildDate }
				, CopyRight{ v.CopyRight }
			{}
		};

		// todo: !!!!!
		//typedef LPASVLOFFSCREEN LPASF_ImageData;

		struct SingleFaceInfo {
			Rect faceRect;			//
			OrientCode faceOrient;	//

			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(SingleFaceInfo);
			SingleFaceInfo(const ASF_SingleFaceInfo& sfi)
				: faceRect{ sfi.faceRect }
				, faceOrient{ static_cast<OrientCode>(sfi.faceOrient) }
			{}
		};

		struct MultiFaceInfo {
			std::vector<Rect> faceRects;			//
			std::vector<int> faceOrients;			//
			std::vector<int> faceIDs;				//

			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(MultiFaceInfo);
			MultiFaceInfo(const ASF_MultiFaceInfo& mfi)
				: faceRects{ mfi.faceRect, mfi.faceRect + mfi.faceNum }
				, faceOrients{ mfi.faceOrient , mfi.faceOrient + mfi.faceNum }
				, faceIDs{ mfi.faceID, mfi.faceID + mfi.faceNum }
			{}

			SingleFaceInfo at(size_t idx) const
			{
				return { { faceRects.at(idx), faceOrients.at(idx)}};
			}

			SingleFaceInfo operator [](size_t idx) const
			{
				return { { faceRects[idx], faceOrients[idx]} };
			}

			SingleFaceInfo front() const
			{
				return { { faceRects.front(), faceOrients.front()} };
			}

			SingleFaceInfo back() const
			{
				return { { faceRects.back(), faceOrients.back()} };
			}

			size_t size() const { return faceRects.size(); }
		};

		struct ActiveFileInfo {
			std::time_t startTime;	//
			std::time_t endTime;	//
			std::string platform;	//
			std::string sdkType;	//
			std::string appId;		//
			std::string sdkKey;		//
			std::string sdkVersion;	//
			std::string fileVersion;//

			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(ActiveFileInfo);
			ActiveFileInfo(const ASF_ActiveFileInfo& i)
				: startTime{ atoll(i.startTime) }
				, endTime{ atoll(i.endTime) }
				, platform{ i.platform }
				, sdkType{ i.sdkType }
				, appId{ i.appId }
				, sdkKey{ i.sdkKey }
				, sdkVersion{ i.sdkVersion }
				, fileVersion{ i.fileVersion }
			{}
		};

		struct LivenessThreshold {
			float thresholdmodelBGR;
			float thresholdmodelIR;

			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(LivenessThreshold);
			LivenessThreshold(const ASF_LivenessThreshold& lt)
				: thresholdmodelBGR(lt.thresholdmodel_BGR)
				, thresholdmodelIR(lt.thresholdmodel_IR)
			{}
		};


		struct FaceFeature : std::vector<std::uint8_t> {
			using std::vector<std::uint8_t>::vector;
			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(FaceFeature);
			FaceFeature(const ASF_FaceFeature&ff)
				: std::vector<std::uint8_t>{ff.feature, ff.feature + ff .featureSize}
			{}
			operator const ASF_FaceFeature() const
			{
				return { const_cast<MByte*>(data()), static_cast<MInt32>(size()) };
			}
		};

		struct AgeInfo : std::vector<int> {
			using std::vector<int>::vector;
			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(AgeInfo);
			AgeInfo(const ASF_AgeInfo& ai)
				: std::vector<int>{ ai.ageArray, ai.ageArray + ai.num }
			{}
		};

		struct GenderInfo : std::vector<int> {
			using std::vector<int>::vector;
			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(GenderInfo);
			GenderInfo(const ASF_GenderInfo& ai)
				: std::vector<int>{ ai.genderArray, ai.genderArray + ai.num }
			{}
		};


		struct Face3DAngle {
			std::vector<float> rolls;
			std::vector<float> yaws;
			std::vector<float> pitchs;
			std::vector<int> statuses;

			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(Face3DAngle);
			Face3DAngle(const ASF_Face3DAngle& f3a)
				: rolls{ f3a.roll , f3a.roll + f3a.num }
				, yaws{ f3a.yaw , f3a.yaw + f3a.num }
				, pitchs{ f3a.pitch , f3a.pitch + f3a.num }
				, statuses{ f3a.status , f3a.status + f3a.num }
			{}
			size_t size() const { return statuses.size(); }
		};


		struct LivenessInfo : std::vector<int> {
			using std::vector<int>::vector;
			OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR(LivenessInfo);
			LivenessInfo(const ASF_LivenessInfo& li)
				: std::vector<int>{ li.isLive, li.isLive + li.num }
			{}
			bool isLiveAt(size_t idx) { return at(idx) == 1; }
			bool isErrorAt(size_t idx) { return at(idx) < 0; }
		};

		/// <summary>
		/// ArcSoft Face Engine
		/// </summary>
		class ArcFaceEngine
		{

		public:
			ArcErrorCode setLivenessParam(const LivenessThreshold& threshold)
			{
				ASF_LivenessThreshold lt{ threshold.thresholdmodelBGR, threshold.thresholdmodelIR };
				return ::ASFSetLivenessParam(engineHandle_, &lt);
			}


			ArcErrorCode process(int width, int height, int format, const std::uint8_t* imgData, const MultiFaceInfo& detectedFaces, ArcEngineMasks combinedMask)
			{
				ASF_MultiFaceInfo amf;
				amf.faceOrient = const_cast<MInt32*>(detectedFaces.faceOrients.data());
				amf.faceRect = const_cast<MRECT*>(static_cast<const MRECT*>(detectedFaces.faceRects.data()));
				amf.faceID = const_cast<MInt32*>(detectedFaces.faceIDs.data());
				amf.faceNum = detectedFaces.size();

				return ::ASFProcess(engineHandle_, width, height, format, const_cast<MUInt8*>(imgData), &amf, combinedMask);
			}

			ArcErrorCode processEx(LPASF_ImageData imgData, const MultiFaceInfo& detectedFaces, ArcEngineMasks combinedMask)
			{
				ASF_MultiFaceInfo amf;
				amf.faceOrient = const_cast<MInt32*>(detectedFaces.faceOrients.data());
				amf.faceRect = const_cast<MRECT*>(static_cast<const MRECT*>(detectedFaces.faceRects.data()));
				amf.faceID = const_cast<MInt32*>(detectedFaces.faceIDs.data());
				amf.faceNum = detectedFaces.size();

				return ::ASFProcessEx(engineHandle_, imgData, &amf, combinedMask);
			}

			ArcErrorCode process_IR(int width, int height, int format, const std::uint8_t* imgData, const MultiFaceInfo& detectedFaces, ArcEngineMasks combinedMask)
			{
				ASF_MultiFaceInfo amf;
				amf.faceOrient = const_cast<MInt32*>(detectedFaces.faceOrients.data());
				amf.faceRect = const_cast<MRECT*>(static_cast<const MRECT*>(detectedFaces.faceRects.data()));
				amf.faceID = const_cast<MInt32*>(detectedFaces.faceIDs.data());
				amf.faceNum = detectedFaces.size();

				return ::ASFProcess_IR(engineHandle_, width, height, format, const_cast<MUInt8*>(imgData), &amf, combinedMask);
			}

			ArcErrorCode ASFProcessEx_IR(LPASF_ImageData imgData, const MultiFaceInfo& detectedFaces, ArcEngineMasks combinedMask)
			{
				ASF_MultiFaceInfo amf;
				amf.faceOrient = const_cast<MInt32*>(detectedFaces.faceOrients.data());
				amf.faceRect = const_cast<MRECT*>(static_cast<const MRECT*>(detectedFaces.faceRects.data()));
				amf.faceID = const_cast<MInt32*>(detectedFaces.faceIDs.data());
				amf.faceNum = detectedFaces.size();

				return ::ASFProcessEx_IR(engineHandle_, imgData, &amf, combinedMask);
			}

			std::optional<FaceFeature> faceFeatureExtractEx(LPASF_ImageData imageData, const SingleFaceInfo& singleFaceInfo)
			{
				FaceFeature feature;
				return faceFeatureExtractEx(feature, imageData, singleFaceInfo)
					? std::make_optional(feature) : std::nullopt;
			}

			ArcErrorCode faceFeatureExtractEx(FaceFeature& feature, LPASF_ImageData imageData, const SingleFaceInfo& singleFaceInfo)
			{
				::ASF_FaceFeature f_;
				ASF_SingleFaceInfo asf{ singleFaceInfo.faceRect,  singleFaceInfo.faceOrient };
				auto ret = ::ASFFaceFeatureExtractEx(engineHandle_, imageData, &asf, &f_);
				feature = FaceFeature(f_.feature, f_.feature + f_.featureSize);
				return ret;
			}

			std::optional<float> faceFeatureCompare(const FaceFeature& f1, const FaceFeature& f2, CompareModel model = CompareModel::CompareModelLifePhoto)
			{
				float confidenceLevel{};
				return faceFeatureCompare(confidenceLevel, f1, f2, model)
					? std::make_optional(confidenceLevel) : std::nullopt;
			}

			ArcErrorCode faceFeatureCompare(float & confidenceLevel, const FaceFeature& f1, const FaceFeature& f2, CompareModel model = CompareModel::CompareModelLifePhoto)
			{
				::ASF_FaceFeature af1 = f1;
				::ASF_FaceFeature af2 = f2;
				return ::ASFFaceFeatureCompare(engineHandle_, &af1, &af2
					, &confidenceLevel, static_cast<ASF_CompareModel>(model));
			}

			std::optional<AgeInfo> getAge()
			{
				AgeInfo ageInfo{};
				return getAge(ageInfo)
					? std::make_optional(ageInfo) : std::nullopt;
			}
			ArcErrorCode getAge(AgeInfo& ageInfo)
			{
				::ASF_AgeInfo asfAgeInfo{};
				auto ret = ::ASFGetAge(engineHandle_, &asfAgeInfo);
				ageInfo = asfAgeInfo;
				return ret;
			}

			std::optional<GenderInfo> getGender()
			{
				GenderInfo genderInfo{};
				return getGender(genderInfo)
					? std::make_optional(genderInfo) : std::nullopt;
			}
			ArcErrorCode getGender(GenderInfo& genderInfo)
			{
				::ASF_GenderInfo asfGenderInfo{};
				auto ret = ::ASFGetGender(engineHandle_, &asfGenderInfo);
				genderInfo = asfGenderInfo;
				return ret;
			}

			std::optional<Face3DAngle> getFace3DAngle()
			{
				Face3DAngle face3DAngle{};
				return getFace3DAngle(face3DAngle)
					? std::make_optional(face3DAngle) : std::nullopt;
			}
			ArcErrorCode getFace3DAngle(Face3DAngle& face3DAngle)
			{
				::ASF_Face3DAngle asfFace3DAngle{};
				auto ret = ::ASFGetFace3DAngle(engineHandle_, &asfFace3DAngle);
				face3DAngle = asfFace3DAngle;
				return ret;
			}

			std::optional<LivenessInfo> getLivenessScore()
			{
				LivenessInfo livenessInfo{};
				return getLivenessScore(livenessInfo)
					? std::make_optional(livenessInfo) : std::nullopt;
			}
			ArcErrorCode getLivenessScore(LivenessInfo& livenessInfo)
			{
				::ASF_LivenessInfo asfLivenessInfo{};
				auto ret = ::ASFGetLivenessScore(engineHandle_, &asfLivenessInfo);
				livenessInfo = asfLivenessInfo;
				return ret;
			}

			std::optional<LivenessInfo> getLivenessScore_IR()
			{
				LivenessInfo livenessInfo{};
				return getLivenessScore_IR(livenessInfo)
					? std::make_optional(livenessInfo) : std::nullopt;
			}
			ArcErrorCode getLivenessScore_IR(LivenessInfo& livenessInfo)
			{
				::ASF_LivenessInfo asfLivenessInfo{};
				auto ret = ::ASFGetLivenessScore_IR(engineHandle_, &asfLivenessInfo);
				livenessInfo = asfLivenessInfo;
				return ret;
			}

		public:

			const ArcVersion static version()
			{
				return ::ASFGetVersion();
			}

			ArcErrorCode static getActiveFileInfo(ActiveFileInfo& afi)
			{
				::ASF_ActiveFileInfo arcAfi{};
				auto ret = ::ASFGetActiveFileInfo(&arcAfi);
				afi = arcAfi;
				return ret;
			}

			std::optional<ActiveFileInfo> static getActiveFileInfo()
			{
				ActiveFileInfo afi{};
				return getActiveFileInfo(afi) ? std::make_optional(afi) : std::nullopt;
			}

			ArcErrorCode static onlineActivation(std::string appId, std::string sdkKey) {
				ArcErrorCode ret = ::ASFOnlineActivation(const_cast<MPChar>(appId.c_str()), const_cast<MPChar>(sdkKey.c_str()));
				onlineActived_ = ret;
				return  ret;
			}

			ArcErrorCode initEngine(DetectMode detectMode,
				OrientPriority	detectFaceOrientPriority,
				int				detectFaceScaleVal,
				int				detectFaceMaxNum,
				ArcEngineMasks	combinedMask)
			{
				auto ret = ::ASFInitEngine(static_cast<ASF_DetectMode>(detectMode)
					, static_cast<ASF_OrientPriority>(detectFaceOrientPriority)
					, detectFaceScaleVal, detectFaceMaxNum
					, combinedMask, &engineHandle_);
				valid_ = (ret == MOK);
				return ret;
			}

			ArcErrorCode detectFaces(MultiFaceInfo& detectedFaces, int width, int height, int format, const std::uint8_t* imgData, DetectModel detectModel = DetectModel::DetectModelRGB)
			{
				ASF_MultiFaceInfo amf;
				auto ret = ::ASFDetectFaces(engineHandle_, width, height, format, const_cast<MUInt8*>(imgData), &amf, static_cast<ASF_DetectModel>(detectModel));
				detectedFaces = amf;
				return ret;
			}

			std::optional<MultiFaceInfo> detectFaces(int width, int height, int format, const std::uint8_t* imgData, DetectModel detectModel = DetectModel::DetectModelRGB)
			{
				MultiFaceInfo mfi{};
				return detectFaces(mfi, width, height, format, imgData, detectModel)
					? std::make_optional(mfi) : std::nullopt;
			}

			ArcErrorCode detectFacesEx(MultiFaceInfo& detectedFaces, LPASF_ImageData imgData, DetectModel detectModel = DetectModel::DetectModelRGB)
			{
				ASF_MultiFaceInfo amf;
				auto ret = ::ASFDetectFacesEx(engineHandle_, imgData, &amf, static_cast<ASF_DetectModel>(detectModel));
				detectedFaces = amf;
				return ret;
			}

			std::optional<MultiFaceInfo> detectFacesEx(LPASF_ImageData imgData, DetectModel detectModel = DetectModel::DetectModelRGB)
			{
				MultiFaceInfo mfi{};
				return detectFacesEx(mfi, imgData, detectModel)
					? std::make_optional(mfi) : std::nullopt;
			}

			ArcErrorCode uninitEngine()
			{
				if (valid_) {
					valid_ = false;
					return ::ASFUninitEngine(engineHandle_);
				}
				return {};
			}


			virtual ~ArcFaceEngine()
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


#undef OHTOAI_DECLARE_DEFAULT_CONSTRUCTOR
}
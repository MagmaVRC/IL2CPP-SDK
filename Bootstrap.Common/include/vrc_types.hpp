#pragma once
#include <cstdint>

namespace Bootstrap {

    enum class InstanceFeatureSetting : uint8_t {
        Emoji = 0,
        Stickers = 1,
        SharingPedestals = 2,
        Prints = 3,
        Drones = 4,
        Props = 5,
    };

    enum class ButtonDesignClass : int32_t {
        Custom = 0,
        Primary = 1,
        Secondary = 2,
        Tertiary = 3,
        Ghost = 4,
        Danger = 5,
        Quick = 6,
        LegacyPrimary = 7,
        LegacySecondary = 8,
        LegacyTertiary = 9,
        LegacyGhost = 10,
        LegacyDanger = 11,
        LegacyQuick = 12,
    };

    enum class UserCameraMode : int32_t {
        Off = 0,
        Photo = 1,
        Stream = 2,
        Animated = 3,
        MultiLayer = 4,
        Print = 5,
        Drone = 6,
    };

    enum class ControllerInputProfile : int32_t {
        Custom = 0,
        Default = 1,
        Gesture = 2,
        Performer = 3,
        Advanced = 4,
        SteamVR = 5,
    };

    enum class OnspSpatializerParameter : int32_t {
        Gain = 0,
        UseInvSqr = 1,
        Near = 2,
        Far = 3,
        Radius = 4,
        DisableRfl = 5,
        AmbiStat = 6,
        ReadOnlyGlobalRflEnabled = 7,
        ReadOnlyNumVoices = 8,
        SendLevel = 9,
        Count = 10,
    };

    enum class MenuElementPressScaleMode : int32_t {
        None = 0,
        ScaleCalculatedByRectSize = 1,
        Scale = 2,
        SizeDelta = 3,
    };

    enum class UserSelectionDisplayState : int32_t {
        Undefined = 0,
        NoSelection = 1,
        Tooltip = 2,
        UserHover = 3,
        UserSelected = 4,
        Notification = 5,
    };

    enum class DebugMenuElementType : int32_t {
        Label = 0,
        LabelNoPadding = 1,
        LabelNoRichText = 2,
        Space = 3,
        Button = 4,
        ButtonSized = 5,
        Horizontal = 6,
        Vertical = 7,
        Texture = 8,
    };

    enum class PlayerEvent : uint8_t {
        Awake = 0,
        Join = 1,
        JoinComplete = 2,
        Leave = 3,
        Unknown = 255
    };

    struct PlayerNameplateData {
        uint32_t size;
        void* Class;
        int gameObject_contents;
        int gameObject_subText;
        int gameObject_textContainer;
        int gameObject_mainContainer;
        int gameObject_icon;
        int gameObject_quickStats;
        int gameObject_platform;
        int gameObject_interactionStatus;
        int gameObject_pronouns;
        int gameObject_playerStatusIcons;
        int gameObject_userMuted;
        int gameObject_userVolume;
        int gameObject_listenBlocked;
        int gameObject_earmuffsIcon;
        int gameObject_focusViewIcon;
        int gameObject_devBanner;
        int gameObject_devIcon;
        int gameObject_groupInfo;
        int gameObject_avatarProgress;
        int gameObject_friendIcon;

        // Rewritten nameplate contents. The child GameObjects moved behind a UI
        // fragment, so on those builds the gameObject_* offsets above stay 0 and
        // these are filled instead -- and the other way round on older builds.
        int fragment;
        int content;
        int icons;
        int gameObject_root;
        int positioner;
        int rectTransform;
        int vrcPlayer;
    };

    struct PlayerData {
        uint32_t size;
        void* Class;
        void* Singleton;
        int VRCPlayerApi;
        int VRCPlayer;
        int APIUser;
        int USpeaker;

        void* UserModelClass;
        int userModel;
        int userModelApiUser;

        // Builds that expose LocalPlayer as a static property instead of a field
        // leave Singleton null and fill these in: native code pointer + the
        // MethodInfo* the thunk takes as its only argument.
        void* get_LocalPlayer;
        void* get_LocalPlayerMethod;
    };

    struct VRCPlayerData {
        uint32_t size;
        void* Class;
        void* Singleton;
        int VRCPlayerApi;
        int Player;
        int PlayerNameplate;
        int PoseRecorder;
        int Serializer;
        int SyncPhysics;
        int ApiAvatar;
        int ApiAvatarFallback;

        void* Color_Administrator;
        void* Color_Friend;
        void* Color_Visitor;
        void* Color_NewUser;
        void* Color_User;
        void* Color_KnownUser;
        void* Color_Trusted;
        void* Color_Nuisance;

        int gameObject_nameplate;
        int gameObject_avatar;

        int Animator;

        int shortDisplayname;
        int device;
        int groupId;
        int avatarId;
    };

    struct LoadBalancingClientData {
        void* Class;
        void* OnEvent;
        void* OpRaiseEvent;
        int   photonPeerOffset;             // LoadBalancingClient.field_PhotonPeer_0

        // Singleton holder: a class with a single static Photon.Realtime.LoadBalancingClient field.
        void* SingletonHolderClass;
        int   singletonHolderStaticOffset;  // offset of the static field within static-data

        // PhotonPeer does not inherit PeerBase - it holds one in a field - so the
        // ping is two hops: PhotonPeer.peerBase then PeerBase.roundTripTime
        // (Single, ms).
        void* PhotonPeerClass;
        int   peerBaseOffset;
        int   roundTripTimeOffset;
    };

    struct FlatBufferNetworkSerializerData {
        void* Class;
        int RequireFastRate;
    };

    struct PhotonViewData {
        void* Class;
        int ViewId;
    };

    struct VRCPlayerStepHeightData {
        void* Class;
        int maxStepHeight;
        int stepUpThreshold;
        int offsetDistance;
        int characterController;
        int groundedLayers;
    };

    struct VRCMotionStateData {
        void* Class;
        int characterController;
    };

    struct ONSPAudioData {
        void* Class;
        int audioSource;
        int onspLowPassFilter;
        int onspAudioSource;
    };

    struct RoomManagerData {
        void* Class;
        int apiWorld;
        int apiWorldInstance;
    };

    struct AssetBundleCacheData {
        void* Class;
        // Builds the CachedAssetBundle every download and cache check goes through:
        // Boolean (ApiModel, String url, UInt64 size, out CachedAssetBundle).
        void* BuildCachedAssetBundle;
    };

    struct UIPageData {
        void* Class;
        int pageName;
        int isModal;
        int isTransparent;
        int clearContextWhenClosed;
        int rectTransform;
        int canvasGroup;
    };

    struct VRCRectMask2DData {
        void* Class;
        int mask;
        int clippingEnabled;
    };

    struct MMSettingsPageData {
        void* Class;
        int scrollRect;
        int radioButtonGroup;
        int headerText;
        int searchTextField;
        int noResultsMessage;
        int settingCategories;
    };

    struct MMSettingsCellData {
        void* Class;
        int id;                    // _ID, the section key
        int name;                  // _name, a LocalizableString by value
        int nameField;             // _nameField, the label's TMP component
        int targetObjectToEnable;  // _targetObjectToEnable, the content section
        int targetObjectToDisable;
    };

    struct MenuScrollRectData {
        void* Class;
        int controlName;
        int scrollEnabled;
        int minimumSpeed;
        int allowPropagationToParent;
        int framesDelayForSet;
        int autoClampScroll;
        int routeToParentWhenContentFits;
    };

    struct SidePanelHeaderData {
        void* Class;
        int headerContainer;
        int viewportContainer;
        int buttonHeight;
        int upgradeAccount;
        int exit;
        int logout;
        int linkDiscord;
    };

    struct MenuWingData {
        void* Class;
        int wingType;
        int canvasGroup;
        int canvas;
        int graphicRaycaster;
        int mask;
    };

    struct SettingsControlData {
        void* Class;
        int title;
        int titleContainer;
        int childIndicator;
        int messages;
        int separators;
        int tooltips;
    };

    struct SettingsToggleData {
        void* Class;
        int toggleBinding;
        int toggleSwitch;
        int toggle;
        int canvasGroup;
        int onValueChanged;
    };

    struct SettingsSliderData {
        void* Class;
        int slider;
        int numberText;
        int infiniteSymbol;
        int canvasGroupsWhenDisabled;
        int toggleClickEnabler;
        int toggleSwitch;
        int toggleButton;
        int button;
        int buttonTooltip;
        int onValueChanged;
    };

    struct SettingsOptionSelectorData {
        void* Class;
        int currentOptionText;
        int leftButton;
        int rightButton;
        int optionTooltip;
        int analyticsController;
        int onValueChanged;
    };

    struct ConfirmDialogData {
        void* Class;
        int titleText;
        int topText;
        int middleText;
        int bottomText;
        int confirmButton;
        int cancelButton;
        int confirmText;
        int cancelText;
    };

    struct InQueueWidgetData {
        void* Class;
        int root;
        int header;
        int exitQueueButton;
        int instanceQueueButton;
        int centerContainerButton;
        int timeInQueueText;
        int queuePositionText;
    };

    struct HudControllerData {
        void* Class;
        int notification;
        int move;
        int userEventCarousel;
        int hudCanvasGroup;
        int gestureOnIcon;
        int gestureOffIcon;
        int controllerHUDIconParent;
        int specialNotification;
        int hypeTrainPanel;
        int notificationSound;
        int pixelShiftingMaterials;
        int hudPortalModeControls;
        int hudPortalProximityWarning;
        int hudAskToPortalPrompt;
        int placementHudControls;
    };

    struct UserCameraData {
        void* Class;
        int lensRenderer;
        int printLensRenderer;
        int lensParent;
        int lensUpIndicator;
        int speakerOnViewfinder;
        int speakerOnLens;
        int buttonSound;
        int timerButton;
    };

    struct PortalInternalData {
        void* Class;
        int selector;
        int clElements;
        int invalidDestinationElements;
        int proximityDestinationElements;
        int cosmeticsHandler;
        int worldText;
        int ownerText;
        int accessText;
        int groupText;
        int ageGateText;
        int playerCountText;
        int timerText;
        int platformIcons;
    };

    struct PhotonRoomOptionsData {
        void* Class;
        int isVisible;
        int isOpen;
        int maxPlayers;
        int playerTtl;
        int emptyRoomTtl;
        int cleanupCacheOnLeave;
        int customRoomProperties;
    };

    struct PhotonAuthValuesData {
        void* Class;
        int authType;
        int authGetParameters;
        int authPostData;
        int token;
        int userId;
    };

    struct PhotonTypedLobbyData {
        void* Class;
        int name;
        int type;
    };

    struct AvatarDetailsPageData {
        void* Class;
        int avatarPanel;
        int authorDropdown;
        int authorCellParent;
        int authorCell;
        int image;
        int nameField;
        int authorField;
        int favoriteButtonText;
        int lastUpdatedText;
        int avatarDescriptionPanel;
        int contentWarnings;
        int tagPrefab;
        int contentWarningsHeader;
        int contentWarningBanner;
        int platformPrefab;
        int supportedPlatformsListBinding;
        int supportedPlatformTags;
        int wearAvatarButton;
        int favoriteButton;
        int favoriteButtonIcon;
        int documentationButton;
        int modelSwitcherToggle;
        int reportButton;
        int favoriteFlow;
    };

    struct AvatarMenuPageData {
        void* Class;
        int selectedAvatarPanel;
        int exploreAvatarsPanel;
        int avatarCollectionsPanel;
        int currentAvatarPanel;
        int tabSelector;
    };

    struct AvatarPerformanceDetailsData {
        void* Class;
        int avatarPanel;
        int overallPerformanceRatingIcon;
        int overallPerformancePreviewText;
        int overallPerformance;
        int trianglesField;
        int boundsField;
        int skinnedMeshesField;
        int meshesField;
        int materialSlotsField;
        int textureMemoryField;
        int physBoneComponents;
        int physBoneTransforms;
        int physBoneColliders;
        int physBoneCollisionCheckCount;
        int contactCount;
        int constraintsField;
        int constraintDepthField;
        int animatorsField;
        int bonesField;
        int lightsField;
        int particleSystemsField;
        int totalMaxParticlesField;
        int meshParticleTrianglesField;
        int particleTrailsField;
    };

    struct AvatarPerformanceLimitPageData {
        void* Class;
        int instancePerformanceLimitText;
        int avatarPerformanceLimitText;
        int instancePerformanceLimitTextStyle;
        int avatarPerformanceTextStyle;
        int instancePerformanceLimitIcon;
        int avatarPerformanceIcon;
    };

    struct AvatarPreviewData {
        void* Class;
        int tempAvatarPrefab;
        int errorAvatarPrefab;
        int filteredAvatarPrefab;
        int safetyAvatarPrefab;
        int maleAnims;
        int femaleAnims;
        int scaleToPedestal;
        int avatarScaleFactor;
    };

    struct AvatarsPageData {
        void* Class;
        int selectedAvatarPanel;
        int avatarCollectionsPanel;
        int shopButton;
        int currentWornHeader;
        int currentWornName;
        int compactToggle;
        int compactToggleImage;
        int compactOnSprite;
        int compactOffSprite;
        int avatarListView;
        int tabSelector;
        int avatarLooksPanel;
        int accessoriesSidebarItem;
    };

    struct CameraDollyPointData {
        void* Class;
        int pointModel;
        int anchorModel;
        int speedSlider;
        int durationSlider;
        int zoomSlider;
        int exposureSlider;
        int focalDistanceSlider;
        int apertureSlider;
        int lookAtMeXOffsetSlider;
        int lookAtMeYOffsetSlider;
        int hueSlider;
        int lightnessSlider;
        int saturationSlider;
        int motionFoldout;
        int focusFoldout;
        int lookAtMeFoldout;
        int greenscreenFoldout;
        int sliderContent;
        int sliderFoldoutArrows;
        int dofSliderContentTitle;
        int dofSliderContentRect;
        int zoomSliderParentRect;
        int focalDistanceSliderParent;
        int apertureSliderParent;
    };

    struct CameraTimerButtonData {
        void* Class;
        int iconTakePhoto;
        int iconStopPhoto;
        int button;
        int icon;
        int disabled;
        int label;
    };

    struct ComfortAndSafetySettingsSectionData {
        void* Class;
        int comfortTurning;
        int personalSpace;
        int afkDetection;
        int advancedViveControls;
        int holoportLocomotion;
        int thirdPersonRotation;
        int locomotionAndTunneling;
        int fingerTrackLocomotion;
        int streamerMode;
        int showCommunityLabs;
        int allowUntrustedUrls;
        int chatboxProfanityFilterSetting;
        int portalPrompt;
        int compatibilityWarning;
        int allowMutualsToSeeSharedConnections;
        int allowDiscordFriends;
        int portalMode;
        int homeInstanceType;
        int homeRegion;
        int contentGatingInfo;
        int contentGatingForced;
        int blockSex;
        int blockAdult;
        int blockViolence;
    };

    struct CreateGroupInstancePanelData {
        void* Class;
        int parent;
        int createInstanceButton;
        int changeGroupButton;
        int changeRolesButton;
        int changeInstanceTypeButton;
        int changeInstanceTypeArrow;
        int setDefaultSettingsToggle;
        int queueEnabledToggle;
        int selectedGroupIcon;
        int selectedGroupText;
        int selectedInstanceTypeText;
        int selectedRolesText;
        int regionToggles;
        int changeRolesArrow;
        int roleSelectionParent;
        int roleSelectionSeparator;
        int ageGateEnabledParent;
        int ageGateEnabledToggle;
        int ageGateEnabledTooltip;
        int ageGateEnabledSeparator;
        int linkedEventSeparator;
        int linkedEventTitle;
        int linkedEventList;
        int linkedEventListRadioButtonPrefab;
    };

    struct ExploreAvatarsPanelData {
        void* Class;
        int filterButton;
        int scrollRect;
        int avatarCategoriesList;
        int avatarCategoryPrefab;
        int avatarCellPrefab;
        int avatarCategoriesContainer;
        int selectedAvatarPanel;
        int timeToRefresh;
        int avatarSyncSize;
        int sortDropdown;
        int analyticsAttribution;
        int analyticsViewMoreAttribution;
        int analyticsViewMoreSubstringAttribution;
    };

    struct FlingHelperData {
        void* Class;
        int syncPhysics;
        int pickup;
        int rigidbody;
        int trailRenderer;
    };

    struct FriendsLocationWorldInstanceData {
        void* Class;
        int worldName;
        int gridLayoutGroup;
        int friends;
        int worldInstance;
        int friendPrefab;
        int canvas;
        int raycaster;
        int headerLayoutElement;
        int foldout;
        int instanceHeight;
        int friendHeight;
    };

    struct GestureDisplayHudData {
        void* Class;
        int gestureImages;
        int leftGestureDisplay;
        int rightGestureDisplay;
        int leftGestureDisplayParent;
        int rightGestureDisplayParent;
        int posLeftOnDesktopCameraOpen;
        int posRightOnDesktopCameraOpen;
        int indicatorPosOffsetForDesktop;
        int gestureBarPosDesktop;
    };

    struct GraphicsSettingsSectionData {
        void* Class;
        int graphicsQuality;
        int antiAliasing;
        int mirrorResolution;
        int mirrorResolutionMessage;
        int shadowQuality;
        int shadowDistanceOverrideMessage;
        int lodQuality;
        int thirdPersonView;
        int thirdPersonViewDistance;
        int thirdPersonViewHorizontalOffset;
        int thirdPersonViewVerticalOffset;
        int fieldOfView;
        int portraitFOV;
        int landscapeFOV;
        int fpsType;
        int fpsLimit;
        int mobileFpsLimit;
        int fullScreen;
        int resolution;
        int particlePhysicsQuality;
        int particleLimiter;
        int pixelLightCount;
        int forcedCameraNearDistance;
        int forcedCameraNearDistanceForcedWarningMessage;
    };

    struct GroupInstancesPanelData {
        void* Class;
        int friendListBinding;
        int nonFriendListBinding;
        int friendWorldInstancePrefab;
        int worldInstancePrefab;
        int sortDropdown;
        int emptyObject;
        int emptyLabel;
        int emptyIcon;
        int friendHeader;
        int nonFriendHeader;
        int linkedEventContainerListBinding;
        int linkedEventContainer;
        int linkedEventListPrefab;
        int groupJoinButtons;
    };

    struct HandPinchUIData {
        void* Class;
        int ingertipCanvasGroup;
        int fingertipIndicators;
        int pinchCenterObject;
        int pinchSnapUpperObject;
        int pinchSnapLowerObject;
        int pinchCentroidFill;
        int pinchCentroidOutline;
        int icon;
        int combinedFill;
        int snapUpperOutline;
        int snapUpperFill;
        int snapLowerOutline;
        int snapLowerFill;
        int isLeft;
    };

    struct HandTrackingCoachData {
        void* Class;
        int handCoachAnimator;
        int handCoachHeadTransform;
        int stageCompleteSFX;
        int leftCoachWristTransform;
        int rightCoachWristTransform;
        int leftCoachRenderer;
        int rightCoachRenderer;
        int hmdRenderer;
        int qmPinchForwardAngle;
        int qmCircleKeyForwardAngle;
        int qmPushButtonAngle;
        int locomotionForwardAngle;
        int turningForwardAngle;
        int circleKeyRootTransform;
        int circleSocket;
        int gazeCursor;
        int gazeCursorOutline;
        int lineRenderer;
        int qmCircleKeyTweenCurve;
        int forwardDirectionPreview;
    };

    struct InstanceDetailsPanelData {
        void* Class;
        int closeButton;
        int joinButton;
        int historyButton;
        int clearFieldButton;
        int dropPortalButton;
        int instanceNameInputField;
        int groupInstanceFields;
        int vRCPlusIcon;
        int ageGateIcon;
        int regionIcon;
        int worldNameText;
        int instanceTypeText;
        int groupNameText;
        int portalSubtext;
        int instanceNameInputFieldTooltip;
    };

    struct InstanceTypeSelectionPanelData {
        void* Class;
        int parent;
        int groupInstanceTypeButton;
        int groupInstanceTypeSeparator;
        int groupPlusInstanceTypeButton;
        int groupPlusInstanceTypeSeparator;
        int groupPublicInstanceTypeButton;
    };

    struct MicSensitivityIndicatorData {
        void* Class;
        int micSensitivityAndIndicator;
        int noiseGateText;
        int sensitivitySlider;
        int inverseSensitivitySlider;
        int volumeSliderSmoothed;
        int volumeSliderRaw;
        int volumeFillRaw;
    };

    struct MMAudioSettingsSectionData {
        void* Class;
        int masterVolumeSetting;
        int uiMenuVolumeSetting;
        int worldVolumeSetting;
        int voicesVolumeSetting;
        int avatarsVolumeSetting;
        int droneVolumeSetting;
        int userVolumeOverridesSetting;
    };

    struct MMSettingsMicrophoneSectionData {
        void* Class;
        int changeMicrophone;
        int micOutputVolume;
        int noiseSuppression;
        int microphoneBehavior;
        int worldJoinToggleBehavior;
        int micIconVisibility;
        int micTestButton;
        int micTestPlayAgain;
        int micTestVisualizer;
    };

    struct MMSettingsPageTrackingAndIKData {
        void* Class;
        int bodyTrackingSection;
        int measureHeight;
        int metricUnitsAdjustment;
        int useLegacyIKSolving;
        int disableShoulderTracking;
        int freezeTrackingOnDisconnect;
        int shoulderWidthCompensation;
        int locomotionAnimationSetting;
        int trackerPrediction;
        int armVsHeightRatio;
        int wristAngle;
        int kneeAngle;
        int forceEyetrackingRaycast;
        int displayEyetrackingDebug;
        int disableEyeLookTracking;
        int disableEyeLidTracking;
        int micMuteDisablesEyetracking;
        int cameraTrackingSection;
        int enableSelfieFaceTracking;
        int enableSelfieFaceTrackingVrcPlus;
        int enableSelfieFaceTrackingAutoQuality;
        int selfieFaceTrackingQualityLevel;
        int enableSelfieHandTracking;
        int selfieExpressionRecenterSpeed;
    };

    struct ModToolsPanelData {
        void* Class;
        int blockedUsersFoldout;
        int blockedUsers;
        int blockedByUsersFoldout;
        int blockedByUsers;
    };

    struct PhotoSharePageData {
        void* Class;
        int shareDirect;
        int dropPrintCanvasGroup;
        int dropPrint;
        int printAsPrint;
        int copyUrlButton;
        int shareAsPedestal;
        int loadingOverlay;
        int closeButton;
        int pageRect;
        int selectShareMethodCanvasGroup;
        int selectShareMethodRect;
        int selectShareMethodTwoButtonSize;
        int selectShareMethodThreeButtonSize;
        int headerText;
        int selectUsersCanvasGroup;
        int userCellPrefab;
        int searchUsersButton;
        int userListBinding;
        int userList;
        int shareButton_Direct;
        int cancelButton_Direct;
        int defaultUserSearchText;
        int userSearchText;
        int contentPedestalOptionsCanvasGroup;
    };

    struct PortalCosmeticsHandlerData {
        void* Class;
        int mobileOpaqueShader;
        int isInsidePreview;
        int portalUITransform;
        int portalPlatformBadgeTransform;
        int portalLabsBadgeTransform;
    };

    struct PrintControllerData {
        void* Class;
        int controlsParent;
        int cancelButton;
        int retakeButton;
        int addNoteButton;
        int printAndSaveButton;
        int printAndSaveButtonLabel;
        int saveEditedPrintButton;
        int display;
        int printCreationCameraPrefab;
        int printPositionTransform;
        int printScaleTransform;
        int printFinalPositionTransform;
        int printingOverlayCanvasGroup;
        int printCameraSkin;
        int lens;
        int printLens;
        int printOverlayText;
    };

    struct PrintDisplayData {
        void* Class;
        int photoDisplay;
        int existingPrintDisplay;
        int noteLabel;
        int authorLabelParent;
        int authorLabel;
        int worldLabelParent;
        int worldLabel;
        int dateLabel;
        int dateLabel_NoWorld;
        int aspectRatioFitter;
    };

    struct PrintObjectData {
        void* Class;
        int printRenderer;
        int canvas;
        int frontCanvasGroup;
        int backCanvasGroup;
        int pickup;
        int preview;
        int printCollider;
        int streamerModeTexture;
        int backCanvasMovableTransform;
        int backCanvasZOffsetStill;
        int backCanvasZOffsetMoving;
        int backCanvasZOffsetMovingDoubleRow;
        int printDuration;
        int scaleDuration;
        int developDuration;
        int targetScale;
        int printerScaleDuration;
        int printerTransform;
        int velocityMultiplier;
        int velocityLerpSpeed;
        int maxVelocity;
        int editNoteButton;
        int saveToGalleryButton;
        int downloadButton;
    };

    struct RadialActionMenuData {
        void* Class;
        int dividerPrefab;
        int optionPrefab;
        int dividerContainer;
        int optionsContainer;
        int mainGraphic;
        int selectionGraphic;
        int selectionObj;
        int optionsCanvasGroup;
        int sounds;
    };

    struct ReportPageData {
        void* Class;
        int submitButton;
        int closeButton;
        int backButton;
        int header;
        int title;
        int submitButtonText;
        int submitButtonTooltip;
        int reportingInfoObject;
        int whatsReportedText;
        int togglePrefab;
        int selectionGroup;
        int categoryList;
        int reasonsList;
        int warningsGroup;
        int warningsList;
        int descriptionFieldContainer;
        int descriptionField;
        int copyrightPanel;
        int copyrightButton;
        int reportFiledObject;
        int reportFiledText;
        int selfModerateButton;
        int selfModerateButtonText;
        int referButton;
    };

    struct SelectedAvatarPanelData {
        void* Class;
        int avatarBounds;
        int avatarPartBounds;
        int avatarUnavailableBanner;
        int avatarPanel;
        int impostorPanel;
    };

    struct SharableContentPedestalData {
        void* Class;
        int sphere;
        int pedestalParent;
        int destroyParticlesPrefab;
        int contentSphereAnimator;
        int vrcPlusCalloutParent;
        int vrcPlusCalloutLabel;
        int typeLabel;
        int typeIcon;
        int labelParent;
        int ownerIconImageBinding;
        int ownerLabel;
        int labelLayoutElement;
        int canvasParent;
        int controlsParent_Receiver;
        int controlsParent_Owner;
        int openMoreOptions;
        int moreOptionsParent;
        int moveButton;
        int destroyButton;
        int hideButton;
        int hideButtonIcon;
        int destroyLocalButton;
        int reportButton;
    };

    struct SocialPageData {
        void* Class;
        int scrollRect;
        int status;
        int statusIcon;
        int search;
        int buttonGroup;
        int buttons;
        int headerText;
        int cellSizeToggle;
        int cellSizeToggleIcon;
        int collapsedCellImage;
        int expandedCellImage;
        int groupNameInputField;
        int sortDropdown;
        int sortStyleElement;
        int sortToolTip;
        int friendFilterButton;
        int groupActivity;
    };

    struct SyncPhysicsData {
        void* Class;
        int hideChildCanvasesOnBlockedUserHeld;
        int forceKinematicOnRemote;
    };

    struct TrackingCalibrationSettingsSectionData {
        void* Class;
        int userRealHeightSetting;
        int avatarMeasurementSetting;
        int allowFBTSetting;
        int calibrateFBTSetting;
        int calibrateTrackersSetting;
        int calibrateTrackersSeparator;
        int fbtSpineModeSetting;
        int locomotionAnimationSetting;
        int autoCenterOSCTrackersSetting;
        int adjustmentButtons;
        int advancedOptionsFoldout;
    };

    struct UserCameraMenuData {
        void* Class;
        int mainGroupScroll;
        int mainGroupContentHolder;
        int mainMenuArrow;
        int selectedColor;
        int deselectedColor;
        int cameraAlert;
        int menuAnimationTime;
        int closeButton;
        int desktopStreamToggle;
        int cameraEarsToggle;
        int streamResolutionMenu;
        int flipButton;
        int flipButtonIcon;
        int flipButtonIconAttached;
        int flipButtonIconDetached;
        int resetZoomButton;
        int toggleOrientationButton;
        int resolutionButton;
        int anchorReset;
        int mainControlGroup;
        int saveControlGroup;
        int menuControlGroup;
        int cameraFlash;
        int sliderLayout;
    };

    struct UserCameraModelData {
        void* Class;
        int remoteShape;
        int lensMesh;
        int upIndicatorMesh;
        int speaker;
        int screenshotSound;
        int timerSound;
        int lineRender;
        int lineRenderCone;
        int nameplate;
        int syncPhysics;
        int lineRenderMaterialPC;
        int lineRenderMaterialMobile;
        int timerCanvas;
        int timerCircle;
        int lens;
        int printLens;
        int defaultMeshEmissionColor;
        int defaultUpIndicatorColor;
        int effectMeshEmissionColor;
        int effectUpIndicatorColor;
        int nameplateOffsetTracked;
        int nameplateOffsetDefault;
        int applyTrackedNameplateOffset;
    };

    struct VRCAudioManagerData {
        void* Class;
        int mixer;
        int referenceONSPConversionVoiceSource;
        int referenceONSPConversionWorldSource;
        int allowOverrideFromRemoteConfig;
        int nearFieldILDNudgeEarRadius;
        int nearFieldILDNudgeEarTranslate;
        int nearFieldILDNudgeDistance;
        int nearFieldILDNudge;
        int nearFieldILDNudgeOnEarBoost;
        int nearFieldILDNudgeOffEarDuck;
        int nearFieldGlobalBoostGain;
        int perEarDirectionalityFadeDistance;
        int perEarDirectionalityEarRadius;
        int perEarDirectionalityMaxScale;
        int perEarDirectionalityPCFactor;
        int trackingScaleMultiplier;
        int trackingScaleMin;
        int trackingScaleMax;
    };

    struct VRCInputManagerHandTrackingData {
        void* Class;
        int gestureIcons;
        int actionMenuGestureIcon;
        int locomotionStart;
        int locomotionEnd;
        int gestureStart;
        int gestureEnd;
        int handPinchUIPrefab;
        int circleKeyMenuOpenerPrefab;
        int wristButtonMenuOpenerPrefab;
    };

    struct VRComfortAndHapticsPageData {
        void* Class;
        int generalSection;
        int holoportLocomotion;
        int thirdPersonRotation;
        int locomotionAndTunneling;
        int responsiveMenu;
        int reduceAnimations;
        int hapticsSection;
        int uiHaptics;
        int pickupHaptics;
        int avatarTouchHaptics;
        int avatarTouchStrength;
        int avatarTouchSensitivity;
        int hapticsWarningMessage;
    };

    struct VRComfortSectionData {
        void* Class;
        int holoportLocomotionSetting;
        int locomotionTunnelingSetting;
        int responsiveMenuSetting;
    };

    struct VRCTrackingSteamData {
        void* Class;
        int drawPalmGizmos;
        int drawHandTrackingSkeleton;
        int drawPersistentSkeleton;
    };

    struct VRCTrackingXRData {
        void* Class;
        int leftPointerGeneric;
        int rightPointerGeneric;
        int leftPointerPico;
        int rightPointerPico;
        int controllerModelLeftPico;
        int controllerModelRightPico;
        int controllerModelLeftPico4Ultra;
        int controllerModelRightPico4Ultra;
        int controllerModelLeftVive;
        int controllerModelRightVive;
        int controllerUIPrefabLeftPico;
        int controllerUIPrefabRightPico;
        int controllerUIPrefabLeftPico4Ultra;
        int controllerUIPrefabRightPico4Ultra;
        int controllerUIPrefabLeftVive;
        int controllerUIPrefabRightVive;
    };

    struct WorldDetailsPanelData {
        void* Class;
        int worldInstanceInfoPane;
        int instancesListBinding;
        int tagsListBinding;
        int contentWarningsListBinding;
        int instancesSortByDropdown;
        int viewMorePrefab;
        int instancePrefab;
        int worldPrefab;
        int tagPrefab;
        int textCellPrefab;
        int tagsHeader;
        int contentWarningsHeader;
        int goButton;
        int dropPortalButton;
        int newInstanceButton;
        int linkEventButton;
        int preloadWorldButton;
        int addToPlaylistButton;
        int setHomeButton;
        int viewOnWebsiteButton;
        int resetUserDataButton;
        int closeInstanceButton;
        int instanceClosed;
        int youAreHere;
    };

    struct WorldInstanceFlowHandlerData {
        void* Class;
        int parentModal;
        int backButton;
        int loadingDisplay;
        int instanceTypeSelectionCG;
        int instanceTypeSelectionCanvasToggle;
        int blockedCG;
        int blockedCanvasToggle;
        int groupSelectionCG;
        int groupSelectionCanvasToggle;
        int groupListBinding;
        int groupButtonPrefab;
        int noGroupsGa;
        int groupInstanceTypeCG;
        int groupInstanceTypeCanvasToggle;
        int instanceTypeSelectionPanel;
        int roleSelectionCG;
        int roleSelectionCanvasToggle;
        int roleSelectionManager;
        int createGroupInstanceCG;
        int createGroupInstanceCanvasToggle;
        int createGroupInstancePanel;
        int performanceLimitSelectionCG;
        int performanceLimitSelectionCanvasToggle;
        int performanceLimitSelectionPanel;
    };

    struct AccessoriesPanelData {
        void* Class;
        int pedestal;
        int applyButton;
        int editButton;
        int nameText;
        int subText;
    };

    struct AccessoryCategoriesPanelData {
        void* Class;
        int accessoryCategoriesBinding;
        int accessoryProductCategoryPrefab;
    };

    struct AgeVerificationStatusButtonData {
        void* Class;
        int button;
        int ageVerificationStatus;
        int icon;
    };

    struct AlertToastData {
        void* Class;
        int alertText;
        int alertSpinner;
        int canvasGroup;
    };

    struct AuthorWorldsPanelData {
        void* Class;
        int authorWorldsListBinding;
        int viewMorePrefab;
        int authorCell;
        int worldPrefab;
    };

    struct AvatarCollectionsPanelData {
        void* Class;
        int avatarListSelectorListBinding;
        int avatarListSelectorPrefab;
        int collectionSelectorGroup;
        int avatarListView;
        int avatarListLockedBanner;
        int sortDropdown;
        int editName;
        int collectionNameDisplay;
        int collapsableStyleComponentController;
        int looksFoldout;
        int avatarsFoldout;
        int looksSeparator;
        int looksFoldoutsContainer;
        int avatarsFoldoutsContainer;
        int avatarListPanel;
        int selectedAvatarPanel;
    };

    struct AvatarListViewData {
        void* Class;
        int layoutGroup;
        int prefabCompact;
        int fadeTarget;
    };

    struct AvatarLooksPanelData {
        void* Class;
        int lookCellsBinding;
        int previewPanel;
        int exploreMarketplaceButton;
        int addCellPrefab;
        int lookCellPrefab;
        int infoIcon;
        int copyIcon;
        int deleteIcon;
    };

    struct AvatarPedestalDisplayData {
        void* Class;
        int invalidAvatarGraphics;
        int loadingTexture;
    };

    struct AvatarSidePanelData {
        void* Class;
        int contentWarnings;
        int tagPrefab;
        int contentWarningsHeader;
        int contentWarningBanner;
        int favoriteButton;
        int favoriteButtonIcon;
        int favoriteAvatarTooltip;
        int favoriteFlow;
        int favoriteButtonText;
        int favoritesVRCPlusIndicator;
        int reportButton;
        int avatarStats;
        int overallPerformanceRatingIcon;
        int polygonsField;
        int boundsField;
        int skinnedMeshesField;
        int meshesField;
        int materialSlotsField;
        int textureMemoryField;
        int physBoneComponents;
        int physBoneTransforms;
        int physBoneColliders;
        int physBoneCollisionCheckCount;
        int contactCount;
    };

    struct AvatarUnavailableBannerData {
        void* Class;
        int type;
        int titleText;
        int descriptionText;
    };

    struct BadgeData {
        void* Class;
        int badgeImage;
        int tooltip;
        int quantityParent;
        int quantityText;
    };

    struct BannerDisplayData {
        void* Class;
        int imageBinding;
        int imageRectTransform;
        int animateOutProperties;
        int animateInProperties;
    };

    struct BannerGroupData {
        void* Class;
        int highlightBanners;
        int defaultBanner;
    };

    struct BundlePacksViewData {
        void* Class;
        int bundleImage;
        int bundleName;
        int bundleExpirationDate;
        int claimButtonParent;
        int claimButton;
        int subscribeButtonParent;
        int claimingBundleOverlay;
        int viewPrevBundleButton;
        int viewNextBundleButton;
        int noBundlesOverlay;
    };

    struct ButtonFooterData {
        void* Class;
        int text;
        int button;
        int tooltip;
        int buttonText;
    };

    struct ButtonLayoutHandlerData {
        void* Class;
        int buttonSpacing;
        int center;
        int allButtons;
        int ownerButtons;
        int ownerNonAuthorButtons;
        int nonOwnerButtons;
        int instanceModeratorButtons;
        int worldDetailsButton;
    };

    struct CalendarEventPanelData {
        void* Class;
        int eventList;
        int headerPrefab;
        int noEventsPanel;
        int noEventsText;
        int iconBinding;
        int loadingOverlay;
    };

    struct CalendarMonthPanelData {
        void* Class;
        int calendarDaysList;
        int calendarDatePrefab;
    };

    struct CalendarSideListData {
        void* Class;
        int listBinding;
        int listItemPrefab;
        int noEventsText;
    };

    struct CameraUIUprightElementData {
        void* Class;
        int polesMarginDegrees;
        int objectToDisableInPortrait;
    };

    struct CampaignPanelData {
        void* Class;
        int campaignTitleText;
        int campaignTitleShadowText;
        int campaignInfoText;
        int learnMoreToMM;
        int confetti;
        int context;
    };

    struct ClockControlData {
        void* Class;
        int clockText;
        int clockDesignatorText;
        int clockTextBackwards;
        int clockDesignatorTextBackwards;
    };

    struct CollapsableStyleComponentControllerData {
        void* Class;
        int sidePanelToggleButton;
        int sidePanelToggleButtonImage;
        int collapsedIconTexture;
        int expandedIconTexture;
        int navBtnToggleTooltip;
    };

    struct CollapsibleSectionData {
        void* Class;
        int expandButton;
        int collapseButton;
        int contentContainer;
        int isExpandedByDefault;
    };

    struct ColorPickerData {
        void* Class;
        int colorWheel;
        int indicator;
        int valueBackground;
        int valueSlider;
    };

    struct ColorPickerHSVData {
        void* Class;
        int gradientDisplay;
        int gradientIndicator;
        int hueDisplay;
        int hueIndicator;
        int indicatorOffsetMulti;
    };

    struct ColorPickerPanelData {
        void* Class;
        int canvasGroup;
        int hexButton;
        int pickerTypeToggle;
        int colorPicker;
        int colorPickerHSV;
    };

    struct ColorThemeEditorPageData {
        void* Class;
        int title;
        int editTitleButton;
        int highlightTab;
        int iconTab;
        int buttonsTab;
        int backgroundTab;
        int textTab;
        int subtextTab;
        int colorPicker;
        int colorPickerHSV;
        int save;
        int close;
        int twoOptionToggle;
        int hexInputButton;
        int shareButton;
        int exportButton;
        int importButton;
    };

    struct CountTextBindingData {
        void* Class;
        int textField;
        int localizableFormat;
        int localizableEmptyText;
        int countFormat;
        int hideIfEmpty;
    };

    struct DataContextSelectorGroupData {
        void* Class;
        int selectDefaultButtonOnStart;
        int buttons;
    };

    struct DebugMenuData {
        void* Class;
        int sidebarTabPrefab;
        int sidebarTabParent;
        int sidebarTabLayoutGroup;
        int sidebarTabContentSizeFitter;
        int contentView;
        int rootCanvas;
        int rootCanvasGroup;
        int uICollider;
        int closeButton;
        int sidebarScrollRect;
        int contentScrollRect;
        int background;
        int backgroundOpacitySlider;
        int localMovementToggle;
        int localMovementDesktopWarning;
        int fPSDisplay;
        int pingDisplay;
        int buildDisplay;
        int debugDisplay;
    };

    struct DebugMenuContentViewData {
        void* Class;
        int labelPrefab;
        int spacePrefab;
        int buttonPrefab;
        int horizontalPrefab;
        int verticalPrefab;
        int togglePrefab;
        int texturePrefab;
        int scrollViewParent;
        int topBarParent;
        int scrollRect;
        int scrollViewLayoutGroup;
        int scrollViewCSF;
        int topBarLayoutGroup;
        int topBarCSF;
        int rootLayoutGroup;
    };

    struct DialogPageData {
        void* Class;
        int footnote;
        int scrollRect;
    };

    struct DofManagerData {
        void* Class;
        int autoFocusCalibrateButton;
        int autoFocusCursorRing;
        int autoFocusCursorFill;
        int screenCanvas;
        int aspectRatio;
    };

    struct DroneAudioVisualData {
        void* Class;
        int velocityScalar;
        int accelScalar;
        int accelMixFactor;
        int accelMixFactorRemote;
        int accelSmoothingTimeSeconds;
        int accelSmoothingTimeSecondsRemote;
        int headpatSpeedLimit;
    };

    struct DroneCustomizationHandlerData {
        void* Class;
        int controller;
        int visualsParent;
        int meshRenderer;
        int trailRenderer;
        int audioVisual;
        int visualsSocket;
        int contactReceiver;
        int insideCustomizationView;
    };

    struct DronePresetsHolderData {
        void* Class;
        int canvasGroup;
        int presetSubmenuButton;
        int scrollLeftArrow;
        int scrollRightArrow;
        int scrollRect;
        int cameraTilt;
        int maxAngleSlider;
        int expoPitchSlider;
        int expoRollSlider;
        int expoYawSlider;
        int expoThrottleSlider;
        int centerSensitivityPitchSlider;
        int centerSensitivityRollSlider;
        int centerSensitivityYawSlider;
        int centerSensitivityThrottleSlider;
        int maxRatePitchSlider;
        int maxRateRollSlider;
        int maxRateYawSlider;
        int maxRateThrottleSlider;
        int modeChangeSmoothingSlider;
        int massSlider;
        int dragSlider;
        int crossSectionAreaSlider;
        int motorPowerSlider;
    };

    struct DroneSettingsPanelData {
        void* Class;
        int canvasGroup;
        int mainControllerTypeLabel;
        int keyboardAndMouseControllerTypeLabel;
        int screenMovementModeLabel;
        int controllerAngleThrottleBehaviorLabel;
        int controllerAcroThrottleBehaviorLabel;
        int controllerInversionLabel;
        int controllerRelativeMouseModeLabel;
        int deadzoneSlider;
        int controllerKBSensitivitySlider;
        int controllerMouseSensitivitySlider;
        int controllerMouseRelativeRateSlider;
        int controllerMouseRelativeCurveSlider;
        int controllerMousePowerCurveSlider;
        int fpvScreenSizeSlider;
        int fpvScreenDimmingSlider;
        int audioFeedbackSlider;
    };

    struct DroneTooltipData {
        void* Class;
        int twinStickTooltip1;
        int twinStickTooltip2;
        int leftStickTextY;
        int leftStickTextX;
        int rightStickTextY;
        int rightStickTextX;
        int mouseAndKeyboardTooltip1;
        int mouseAndKeyboardTooltip2;
        int wsText;
        int adText;
        int mouseYAxisText;
        int mouseXAxisText;
    };

    struct EmojiMaskButtonData {
        void* Class;
        int display;
        int button;
        int style;
    };

    struct EmojiMaskPickerData {
        void* Class;
        int buttonPrefab;
        int maskedListBinding;
        int overlayDisplay;
    };

    struct FeatureCalloutData {
        void* Class;
        int key;
        int target;
        int button;
        int toggle;
        int isVRCPlusOnly;
        int alignment;
        int offset;
    };

    struct GroupActivityCellData {
        void* Class;
        int groupName;
        int groupIcon;
        int gridLayoutGroup;
        int instances;
        int canvas;
        int raycaster;
        int headerLayoutElement;
        int foldout;
        int groupInstanceContentPrefab;
        int viewMoreButtonPrefab;
        int groupInfoButton;
    };

    struct GroupAnnouncementsPanelData {
        void* Class;
        int groupInfoPanel;
        int groupAnnouncementPanel;
        int announcementsListBinding;
        int announcementPrefab;
        int noAnnouncementDisplay;
        int noAnnouncementLabel;
        int noAnnouncementImage;
        int announcementsCanvasToggle;
        int scrollRect;
    };

    struct GroupInfoPanelMemberData {
        void* Class;
        int parentPanel;
        int bannerPanel_Member;
        int memberLanding;
        int openGroupActionMenuButton;
        int nameplateToggle;
        int groupInstances;
        int groupInstancesCanvasToggle;
        int aboutCanvasToggle;
        int postPanel;
        int supportCanvasToggle;
        int groupProducts;
        int groupGallery;
        int groupGalleryCanvasToggle;
        int groupRulesAbout;
        int descriptionTextAbout;
        int groupRulesSection_Member;
        int groupDescriptioinSection_Member;
        int groupMembers;
        int groupMembersCanvasToggle;
        int instancesTabsButtonText;
        int newAnnouncementBadge;
        int eventsCanvasToggle;
        int calendarEventPanel;
        int calendarMonth;
    };

    struct GroupInfoPanelNonMemberData {
        void* Class;
        int bannerPanel_NonMember;
        int nonMemberLanding;
        int descriptionText;
        int groupJoinButtons;
        int groupRules;
        int groupRulesSection;
        int groupDescriptionSection;
        int groupPublicGallery;
        int groupPublicGalleryCanvasToggle;
        int groupProducts;
        int groupPublicSupportCanvasToggle;
        int linksAndLanguagesParent_nonMember;
        int groupBanner;
        int aboutCanvasToggle;
        int aboutLayoutElement;
        int postPanel;
        int groupInstances;
        int groupInstancesCanvasToggle;
        int instancesTabsButtonText;
        int eventsCanvasToggle;
        int calendarEventPanel;
        int calendarMonth;
    };

    struct GroupInstanceRowData {
        void* Class;
        int instanceTypeText;
        int groupNameText;
    };

    struct GroupJoinButtonsData {
        void* Class;
        int requestToJoinGroupButton;
        int joinGroupButton;
        int cancelRequestButton;
        int acceptInviteButton;
        int declineInviteButton;
        int joinGroupIconStyleElement;
        int joinGroupText;
        int joinPendingDisplay;
    };

    struct GroupMemberListData {
        void* Class;
        int listBinding;
        int memberItemPrefab;
        int scrollRect;
    };

    struct GroupProfileButtonData {
        void* Class;
        int imageBinding;
        int button;
        int groupName;
        int tooltip;
    };

    struct GroupRepToggleData {
        void* Class;
        int raycastBlockingObject;
        int tooltip;
        int reppingIcon;
        int notReppingIcon;
        int displayText;
        int iconCanvasGroup;
    };

    struct GroupsDiscoveryPanelData {
        void* Class;
        int groupsListBinding;
        int groupCell;
        int searchButton;
    };

    struct GroupSelectorPanelData {
        void* Class;
        int groupBannerPrefab;
        int groupButtonParent;
        int page;
    };

    struct HeaderResetScrollData {
        void* Class;
        int button;
        int enabled;
    };

    struct HeightFitterData {
        void* Class;
        int rectTransform;
        int heightOffsetTransform;
        int text;
        int textRectTransform;
        int extraHeightPadding;
        int updateNameplate;
    };

    struct HiddenPrintMenuData {
        void* Class;
        int ownerLabel;
        int labelLayoutElement;
        int ownerIconBinding;
        int showButton;
        int reportButton;
        int removeButton;
        int parent;
    };

    struct HudAskToPortalPromptData {
        void* Class;
        int canvasGroup;
        int buttonText;
        int buttonImage;
    };

    struct HudPortalModeControlsData {
        void* Class;
        int canvasGroup;
        int controlsText;
        int portalFailureText;
        int okButtonImage;
        int cancelButtonImage;
    };

    struct HudPortalProximityWarningData {
        void* Class;
        int canvasGroup;
        int promptText;
    };

    struct HypeTrainPanelData {
        void* Class;
        int iconTransform;
        int iconImage;
        int giftIconTransform;
        int giftText;
        int timerText;
        int timerIcon;
        int timerNormalColor;
        int timerMomentumFadingColor;
        int timerMomentumCriticalColor;
        int iconCanvasGroup;
        int giftTextCanvasGroup;
        int timerCanvasGroup;
        int hypeTrainSprite;
        int completedSprite;
    };

    struct ImageSelectorData {
        void* Class;
        int mainImageBinding;
        int previousImageButton;
        int nextImageButton;
        int navigationButtonsCanvasGroup;
        int listBinding;
        int galleryImagePrefab;
        int scrollRect;
        int scrollDuration;
    };

    struct InstanceCreationModalData {
        void* Class;
        int loadingDisplay;
        int worldInstanceFlowHandler;
    };

    struct InstanceGroupCellData {
        void* Class;
        int clickType;
        int groupTabToOpenTo;
    };

    struct ItemsPanelData {
        void* Class;
        int countText;
        int itemsBinding;
        int canvasGroup;
        int itemCellPrefab;
    };

    struct LiveEventsData {
        void* Class;
        int nameText;
        int eventCellPrefab;
        int eventListBinding;
    };

    struct LoadingBarData {
        void* Class;
        int minWidth;
        int text;
        int fill;
        int fillGraphic;
    };

    struct MatManagerData {
        void* Class;
        int customMaterialGraphics;
        int managedRoots;
    };

    struct MenuDragHandleData {
        void* Class;
        int canvasGroup;
        int dragInstructions;
    };

    struct MenuPlacerData {
        void* Class;
        int root;
        int container;
        int quickMenu;
    };

    struct MicTestControllerData {
        void* Class;
        int visualizerMaterial;
        int micTestAudioSource;
    };

    struct MicTestVisualizerData {
        void* Class;
        int visualizer;
        int parentLayoutElement;
        int visualizerHiddenHeight;
        int visualizerShownHeight;
    };

    struct MinimizedKeyboardData {
        void* Class;
        int panelRectTransform;
        int disableWhenMinimized;
        int enableWhenMinimized;
        int submitButton;
        int arrow;
        int minimizeButton;
    };

    struct MobileControlsSettingsSectionData {
        void* Class;
        int lookSensitivity;
        int screenEdgeRotateSpeed;
        int showSecondaryUI;
        int enableAutoWalk;
        int enableAutoHold;
        int invertedLook;
        int enableFocusView;
        int interactionMode;
    };

    struct MuteReminderData {
        void* Class;
        int buttonVisuals;
        int micActiveColor;
        int micMutedColor;
    };

    struct NotificationToastData {
        void* Class;
        int text;
        int display;
    };

    struct NotificationHudData {
        void* Class;
        void* setText;
        void* send;
    };

    struct ParticleSystemSelectorData {
        void* Class;
        int particleSystemSelectorButton;
        int buttonList;
        int scrollView;
    };

    struct ParticleSystemSelectorButtonData {
        void* Class;
        int label;
        int button;
        int style;
    };

    struct PerformanceLimitSelectionPanelData {
        void* Class;
        int parent;
        int confirmButton;
        int performanceRatingGroup;
    };

    struct PinnedHeaderData {
        void* Class;
        int scrollRect;
        int pinnedCategoryHeader;
        int pinnedCategoryHeaderText;
        int graphic;
        int canvasGroup;
    };

    struct PlatformIconsData {
        void* Class;
        int richPlatformCanvas;
        int supportedPlatformsListBinding;
        int supportedPlatformTags;
        int platformPrefab;
    };

    struct PlaylistSidebarData {
        void* Class;
        int vrcPlusPlaylistSidebarItemPrefab;
        int classicPlaylistSidebarItemPrefab;
        int settingsButtonSprite;
        int resultsPage;
        int listBinding;
        int scrollRect;
        int dataContextSelectorGroup;
    };

    struct PopupPanelData {
        void* Class;
        int canvasGroup;
        int verticalLayoutGroup;
    };

    struct PortalDropControlsData {
        void* Class;
        int dropPortalButton;
        int securePortalButton;
        int portalSubtext;
        int portalLockedDisplay;
        int portalUnlockedDisplay;
        int securePortalTooltip;
        int portalButtonTooltip;
    };

    struct PortalPrefabProviderData {
        void* Class;
        int portalPrefab;
        int placeholderPrefab;
    };

    struct PreloadInfoData {
        void* Class;
        int preloadProgressBarContainer;
        int preloadProgressBar;
    };

    struct PreviewButtonData {
        void* Class;
        int button;
        int tryAvatarSprite;
        int wearAvatarSprite;
    };

    struct PrintCreationCameraData {
        void* Class;
        int cam;
        int printPreview;
    };

    struct ProductCategorySectionData {
        void* Class;
        int title;
        int productCellPrefab;
        int contentBinding;
    };

    struct ProductCellData {
        void* Class;
        int title;
        int username;
        int price;
        int tooltip;
        int button;
        int thumbnailBinding;
        int styleElement;
        int expirationTextHolder;
        int expireText;
        int discountTextHolder;
        int discountText;
    };

    struct ProductListPanelData {
        void* Class;
        int listBinding;
        int listCountBinding;
        int productPrefab;
        int sortProductsDropdown;
    };

    struct ProductTypeIconLabelData {
        void* Class;
        int icon;
        int label;
        int background;
        int roleSprite;
        int udonSprite;
        int avatarSprite;
        int itemSprite;
        int stickerSprite;
        int emojiSprite;
        int cosmeticSprite;
        int creditsSprite;
        int accessorySprite;
        int abilityPlayerMovementSprite;
    };

    struct ProfileAvatarImageData {
        void* Class;
        int streamerModeTexture;
        int layoutElement;
    };

    struct PropPlacementCursorData {
        void* Class;
        int indicatorContainer;
        int outlineContainer;
        int propPreview;
        int propPreviewMaterial;
        int propPreviewSuccessColor;
        int propPreviewFailColor;
    };

    struct PurchasesPanelData {
        void* Class;
        int contentTransformToRebuild;
        int oneTimePurchases;
        int purchasesScroll;
        int purchasedItemElementPrefab;
        int sortDropdown;
        int purchasesFilterButton;
        int canvas;
        int raycaster;
    };

    struct ReadMoreTextData {
        void* Class;
        int primaryTextBox;
        int readMoreButton;
        int styleElement;
        int readMoreModalTitle;
        int maxTextLength;
    };

    struct RoleContentCellData {
        void* Class;
        int roleNameText;
        int style;
    };

    struct RoleSelectionManagerData {
        void* Class;
        int selectAllButton;
        int selectAllSelected;
        int managementRolesHeader;
        int vanityRolesHeader;
        int roleListBinding;
        int roleButtonPrefab;
        int selectRolesButton;
        int createWorldInstanceFlowHandler;
    };

    struct RoundedRectData {
        void* Class;
        int background;
        int backgroundInnerStroke;
        int backgroundOuterStroke;
    };

    struct SafetyLevelToggleData {
        void* Class;
        int safetyLevel;
        int tooltipText;
    };

    struct SaveToDiskButtonData {
        void* Class;
        int galleryIcon;
        int checkmarkIcon;
        int loadingDisplay;
        int button;
    };

    struct ScrollAmountManagerData {
        void* Class;
        int rightArrow;
        int leftArrow;
        int buttonParent;
    };

    struct ScrollToTargetData {
        void* Class;
        int target;
        int scrollDuration;
    };

    struct SelectionBindingData {
        void* Class;
        int selectionOverlays;
        int selectionStyleElements;
    };

    struct ShaderFeatureGraphicData {
        void* Class;
        int sprite;
        int sliceScale;
        int shaderFeatureStripes;
        int shaderFeatureRectOffsetChannel;
        int shaderFeatureMasking;
        int maskedImageFadeAmount;
        int maskedImageWidth;
        int maskedImageTexture;
        int gradientColor;
        int useGradient;
        int gradientSizeDelta;
    };

    struct SharableContentSphereData {
        void* Class;
        int cardRenderer;
        int animationTextureSheetPlayer;
        int cardParent;
        int paletteDisplay;
        int paletteObjectsToColor;
        int paletteText;
    };

    struct ShortCodeButtonData {
        void* Class;
        int shortCodeBtn;
        int shortCodeText;
    };

    struct SocialRankIconData {
        void* Class;
        int trustIcon;
        int verifiedIcon;
    };

    struct SpecialNotificationData {
        void* Class;
        int text;
        int icon;
        int border;
        int background;
        int canvasGroup;
    };

    struct SpriteToggleButtonData {
        void* Class;
        int isOn;
        int offSprite;
        int onSprite;
    };

    struct StatusIconData {
        void* Class;
        int onlineIcon;
        int activeIcon;
    };

    struct StoreItemDetailsPanelData {
        void* Class;
        int itemNameLabel;
        int itemImage;
        int itemImageParent;
        int purchaseInfoContainer;
        int purchaseButton;
        int priceText;
        int userData;
    };

    struct TabbedPanelData {
        void* Class;
        int group;
        int tabContainer;
        int contentContainer;
        int tabPrefab;
    };

    struct TimeSliderData {
        void* Class;
        int slider;
        int target;
        int handle;
        int canvasGroup;
    };

    struct ToggleButtonData {
        void* Class;
        int onIconStyle;
        int offIconStyle;
        int tooltip;
        int toggleBinding;
    };

    struct ToggleMenuItemData {
        void* Class;
        int label;
        int toggle;
        int iconImage;
    };

    struct ToggleSwitchData {
        void* Class;
        int handle;
        int turnedOffCanvasGroup;
        int turnedOnCanvasGroup;
        int handleCanvasGroup;
        int tooltip;
        int toggleBinding;
    };

    struct TouchLookData {
        void* Class;
        int interactableMask;
        int playerMask;
        int touchMove;
        int selectRightButton;
    };

    struct TouchMoveData {
        void* Class;
        int stickPosLandscape;
        int stickPosPortrait;
    };

    struct TransformPanelData {
        void* Class;
        int canvasGroup;
        int tabbedPanel;
        int posPanel;
        int rotPanel;
        int scalePanel;
        int sliderPosX;
        int sliderPosY;
        int sliderPosZ;
        int dropdownReparent;
        int buttonReparentHelp;
        int sliderRotX;
        int sliderRotY;
        int sliderRotZ;
        int sliderScaleX;
        int sliderScaleY;
        int sliderScaleZ;
        int sliderScaleAll;
    };

    struct TwoOptionToggleData {
        void* Class;
        int toggle;
        int handle;
        int leftOptionImage;
        int rightOptionImage;
    };

    struct TypingIndicatorData {
        void* Class;
        int canvasGroup;
        int rectTransform;
        int heightFitter;
        int fadeInOutSpeed;
        int typingIndicatorOffset;
        int indicatorText;
        int indicatorTextSpeed;
    };

    struct UIWidgetData {
        void* Class;
        int id;
        int visibility;
        int isClickable;
        int tooltip;
        int hasBackground;
        int backgroundFillMode;
        int backgroundColor;
        int backgroundGradientColorB;
        int backgroundGradientAngle;
        int backgroundSprite;
        int backgroundCornerRadii;
        int backgroundBorderColor;
        int backgroundBorderThickness;
    };

    struct ValidationHandlerData {
        void* Class;
        int button;
        int icon;
        int tooltip;
        int text;
    };

    struct VariablesPanelData {
        void* Class;
        int tabbedPanel;
        int variablesContainer;
        int canvasGroup;
        int scrollRect;
        int sliderPrefab;
        int dropdownPrefab;
        int checkboxPrefab;
        int colorPrefab;
        int colorPickerPanel;
    };

    struct VideoPlayerControlsData {
        void* Class;
        int playImage;
        int playPauseButton;
        int target;
        int timeSlider;
    };

    struct VrcPointerData {
        void* Class;
        int maximumDistance;
        int heightLimitAngle;
        int triggerInteraction;
        int curveRenderer;
        int curveOffset;
        int curveFrequency;
        int hitColor;
        int missColor;
    };

    struct VRCTrackingManagerData {
        void* Class;
        int bodyEstimation;
        int inverseScaleRoot;
    };

    struct VRCUiShapeDebuggerData {
        void* Class;
        int leftCursorText;
        int rightCursorText;
        int imdraw;
    };

    struct WalletMenuData {
        void* Class;
        int processingOverlay;
        int purchasesPanel;
        int cancelProcessingButton;
        int radioButtonSelectorGroup;
        int subscriptionNotifier;
    };

    struct WorldCategoryPickerData {
        void* Class;
        int sidePanelButtonPrefab;
        int dataContextSelectorGroup;
        int worldsPanel;
        int worldLandingMM;
        int sidePanelButtonsBinding;
        int varietyButton;
        int scrollRect;
    };

    struct WorldInformationPanelData {
        void* Class;
        int detailsTab;
        int storeTab;
        int authorTab;
        int detailsPanel;
        int storePanel;
        int authorPanel;
        int originalWorldStorePanel;
        int centralMarketplaceWorldStorePanel;
    };

    struct WorldLandingMMData {
        void* Class;
        int topBanner;
        int list;
        int gridLayoutGroup;
        int worldCellPrefab;
        int randomizeButton;
    };

    struct WorldsMyStuffData {
        void* Class;
        int recentlyVisited;
        int recentlyUpdatedFavorites;
        int authoredSidebarItem;
        int savedSearchSidebarItems;
        int settingsOptionSprite;
        int deleteOptionSprite;
        int scrollRect;
        int contentPanel;
        int dataContextSelectorGroup;
    };

    struct WorldsPanelData {
        void* Class;
        int gridLayoutGroup;
        int bannerCarousel;
        int ipsBannerCarousel;
        int descriptionText;
        int descriptionBG;
        int list;
        int worldCellPrefab;
        int worldCellCompactPrefab;
        int filter;
        int randomizeButton;
        int optInPanel;
        int contentGridPanel;
        int optInButton;
        int cellSizeToggle;
        int cellSizeToggleTooltip;
        int cellSizeToggleIcon;
        int collapsedCellImage;
        int expandedCellImage;
    };

    struct VRCDroneData {
        void* Class;
        int vrcPickup;
        int trailRenderer;
        int worldCollider;
        int pickupCollider;
        int PlayerOwner;
    };

} // namespace Bootstrap

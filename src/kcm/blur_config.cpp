/*
    SPDX-FileCopyrightText: 2010 Fredrik Höglund <fredrik@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "blur_config.h"

//#include <config-kwin.h>

// KConfigSkeleton
#include "blurconfig.h"

#include <KPluginFactory>
#include "kwineffects_interface.h"

#include <QFileDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QMap>
#include <QSignalBlocker>

namespace KWin
{

K_PLUGIN_CLASS(BlurEffectConfig)

struct GlassPreset {
    int blurStrength;
    int noiseStrength;
    double brightness;
    double saturation;
    double contrast;
    QString tintColor;
    QString glowColor;
    bool edgeLighting;
    int refractionStrength;
    int refractionEdgeSize;
    int refractionNormalPow;
    int refractionRGBFringing;
    bool oklabSaturation;
    bool physicallyBasedRefraction;
};

static const QMap<QString, GlassPreset> s_presets = {
    { QStringLiteral("LiquidGlass"), { 48, 3, 1.05, 1.15, 1.00, QStringLiteral("#0dffffff"), QStringLiteral("#40ffffff"), true, 14, 15, 7, 20, true, true } },
    { QStringLiteral("MacosLight"), { 32, 2, 1.15, 1.40, 1.00, QStringLiteral("#1cffffff"), QStringLiteral("#00000000"), false, 0, 10, 4, 0, false, false } },
    { QStringLiteral("MacosDark"), { 36, 2, 0.75, 1.50, 1.10, QStringLiteral("#331c1c1c"), QStringLiteral("#00000000"), false, 0, 10, 4, 0, false, false } },
    { QStringLiteral("WindowsAcrylic"), { 60, 12, 0.90, 1.20, 1.00, QStringLiteral("#2d0c0c0c"), QStringLiteral("#00000000"), false, 0, 10, 4, 0, false, false } },
    { QStringLiteral("FrostyGlass"), { 40, 5, 1.00, 1.00, 1.00, QStringLiteral("#4dffffff"), QStringLiteral("#00000000"), false, 0, 10, 4, 0, false, false } }
};

BlurEffectConfig::BlurEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    BlurConfig::instance("kwinrc");
    addConfig(BlurConfig::self(), widget());

    auto updateRoundedCornerControls = [this]() {
        const bool useDeclaredCornerRadius = ui.kcfg_UseDeclaredCornerRadius->isChecked();
        const bool customCornerRadius = !useDeclaredCornerRadius;
        const bool dynamicCorners = customCornerRadius && ui.kcfg_DynamicCorners->isChecked();

        ui.labelTopCornerRadius->setEnabled(customCornerRadius);
        ui.kcfg_TopCornerRadius->setEnabled(customCornerRadius);
        ui.labelBottomCornerRadius->setEnabled(customCornerRadius);
        ui.kcfg_BottomCornerRadius->setEnabled(customCornerRadius);
        ui.labelMenuCornerRadius->setEnabled(customCornerRadius);
        ui.kcfg_MenuCornerRadius->setEnabled(customCornerRadius);
        ui.labelDockCornerRadius->setEnabled(customCornerRadius);
        ui.kcfg_DockCornerRadius->setEnabled(customCornerRadius);
        ui.kcfg_RoundCornersOfMaximizedWindows->setEnabled(customCornerRadius);
        ui.kcfg_DynamicCorners->setEnabled(customCornerRadius);
        ui.kcfg_DynamicCornersExcludeDocks->setEnabled(dynamicCorners);
        ui.kcfg_DynamicCornersExcludeTooltips->setEnabled(dynamicCorners);
        ui.kcfg_DynamicCornersExcludeMenus->setEnabled(dynamicCorners);
    };
    updateRoundedCornerControls();
    connect(ui.kcfg_UseDeclaredCornerRadius, &QCheckBox::toggled, this, updateRoundedCornerControls);
    connect(ui.kcfg_DynamicCorners, &QCheckBox::toggled, this, updateRoundedCornerControls);

    QFile about(":/effects/glass/kcm/about.html");
    if (about.open(QIODevice::ReadOnly)) {
        const auto html = about.readAll()
            .replace("${title}", dgettext("kwin_effects_glass", "Glass"))
            .replace("${version}", ABOUT_VERSION_STRING)
            .replace("${repo}", "https://github.com/4v3ngR/kwin-effects-glass");
        ui.aboutText->setHtml(html);
    }

    populateProfiles();

    // Select the saved profile
    const QString activeProfile = BlurConfig::activeProfile();
    const int profileIndex = ui.ActiveProfile->findData(activeProfile);
    if (profileIndex != -1) {
        ui.ActiveProfile->setCurrentIndex(profileIndex);
    } else {
        ui.ActiveProfile->setCurrentIndex(ui.ActiveProfile->findData(QStringLiteral("Custom")));
    }

    connect(ui.ActiveProfile, &QComboBox::activated, this, [this](int index) {
        onProfileChanged(ui.ActiveProfile->itemData(index).toString());
    });

    auto connectControl = [this](auto *control, auto signal) {
        connect(control, signal, this, &BlurEffectConfig::onControlModified);
    };

    connectControl(ui.kcfg_BlurStrength, &QSlider::valueChanged);
    connectControl(ui.kcfg_NoiseStrength, &QSlider::valueChanged);
    connectControl(ui.kcfg_Brightness, &QDoubleSpinBox::valueChanged);
    connectControl(ui.kcfg_Saturation, &QDoubleSpinBox::valueChanged);
    connectControl(ui.kcfg_Contrast, &QDoubleSpinBox::valueChanged);
    connectControl(ui.kcfg_TintColor, &QLineEdit::textChanged);
    connectControl(ui.kcfg_GlowColor, &QLineEdit::textChanged);
    connectControl(ui.kcfg_EdgeLighting, &QCheckBox::toggled);
    connectControl(ui.kcfg_RefractionStrength, &QSlider::valueChanged);
    connectControl(ui.kcfg_RefractionEdgeSize, &QSlider::valueChanged);
    connectControl(ui.kcfg_RefractionNormalPow, &QSlider::valueChanged);
    connectControl(ui.kcfg_RefractionRGBFringing, &QSlider::valueChanged);
    connectControl(ui.kcfg_BlurMatching, &QRadioButton::toggled);
    connectControl(ui.kcfg_BlurNonMatching, &QRadioButton::toggled);
    connectControl(ui.kcfg_BlurAllExceptDocksMenus, &QRadioButton::toggled);
    connectControl(ui.kcfg_OklabSaturation, &QCheckBox::toggled);
    connectControl(ui.kcfg_PhysicallyBasedRefraction, &QCheckBox::toggled);

    setupContextualHelp();
}

BlurEffectConfig::~BlurEffectConfig()
{
}

void BlurEffectConfig::populateProfiles()
{
    ui.ActiveProfile->addItem(i18n("Liquid Glass"), QStringLiteral("LiquidGlass"));
    ui.ActiveProfile->addItem(i18n("macOS Light"), QStringLiteral("MacosLight"));
    ui.ActiveProfile->addItem(i18n("macOS Dark"), QStringLiteral("MacosDark"));
    ui.ActiveProfile->addItem(i18n("Windows Acrylic"), QStringLiteral("WindowsAcrylic"));
    ui.ActiveProfile->addItem(i18n("Frosty Glass"), QStringLiteral("FrostyGlass"));
    ui.ActiveProfile->addItem(i18n("Custom / Manual"), QStringLiteral("Custom"));
}

void BlurEffectConfig::onProfileChanged(const QString &profileName)
{
    if (profileName == QStringLiteral("Custom")) {
        return;
    }
    applyPreset(profileName);
    markAsChanged();
}

void BlurEffectConfig::applyPreset(const QString &profileName)
{
    auto it = s_presets.find(profileName);
    if (it == s_presets.end()) {
        return;
    }
    const auto &preset = *it;

    m_updatingFromPreset = true;

    ui.kcfg_BlurStrength->setValue(preset.blurStrength);
    ui.kcfg_NoiseStrength->setValue(preset.noiseStrength);
    ui.kcfg_Brightness->setValue(preset.brightness);
    ui.kcfg_Saturation->setValue(preset.saturation);
    ui.kcfg_Contrast->setValue(preset.contrast);
    ui.kcfg_TintColor->setText(preset.tintColor);
    ui.kcfg_GlowColor->setText(preset.glowColor);
    ui.kcfg_EdgeLighting->setChecked(preset.edgeLighting);
    ui.kcfg_RefractionStrength->setValue(preset.refractionStrength);
    ui.kcfg_RefractionEdgeSize->setValue(preset.refractionEdgeSize);
    ui.kcfg_RefractionNormalPow->setValue(preset.refractionNormalPow);
    ui.kcfg_RefractionRGBFringing->setValue(preset.refractionRGBFringing);
    ui.kcfg_OklabSaturation->setChecked(preset.oklabSaturation);
    ui.kcfg_PhysicallyBasedRefraction->setChecked(preset.physicallyBasedRefraction);

    m_updatingFromPreset = false;
}

void BlurEffectConfig::onControlModified()
{
    if (m_updatingFromPreset) {
        return;
    }
    const int customIndex = ui.ActiveProfile->findData(QStringLiteral("Custom"));
    if (customIndex != -1 && ui.ActiveProfile->currentIndex() != customIndex) {
        QSignalBlocker blocker(ui.ActiveProfile);
        ui.ActiveProfile->setCurrentIndex(customIndex);
        markAsChanged();
    }
}

void BlurEffectConfig::setContextualHelp(
    KContextualHelpButton *const contextualHelpButton,
    const QString &text,
    QWidget *const heightHintWidget
)
{
    contextualHelpButton->setContextualHelpText(text);
    if (heightHintWidget) {
        const auto ownHeightHint = contextualHelpButton->sizeHint().height();
        const auto otherHeightHint = heightHintWidget->sizeHint().height();
        if (ownHeightHint >= otherHeightHint) {
            contextualHelpButton->setHeightHintWidget(heightHintWidget);
        }
    }
}

void BlurEffectConfig::setupContextualHelp()
{
    setContextualHelp(
        ui.windowClassesContextualHelp,
        i18n("<p>Specify one window class per line.</p><p>Use <code>$blank</code> to match empty window classes.<br/>Use <code>$$</code> for literal dollar sign.</p>"),
        ui.windowClassesBriefDescription
    );
}

void BlurEffectConfig::save()
{
    const QString profile = ui.ActiveProfile->currentData().toString();
    BlurConfig::setActiveProfile(profile);
    BlurConfig::self()->save();

    KCModule::save();

    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());

    if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
        interface.reconfigureEffect(QStringLiteral("glass_x11_custom"));
    } else {
        interface.reconfigureEffect(QStringLiteral("glass_custom"));
    }
}

} // namespace KWin

#include "blur_config.moc"

#include "moc_blur_config.cpp"

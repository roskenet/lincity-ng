/* ---------------------------------------------------------------------- *
 * src/lincity/modules/windpower.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#define GROUP_WIND_POWER_COLOUR (green(25))
#define GROUP_WIND_POWER_COST   20000
#define GROUP_WIND_POWER_COST_MUL 25
#define GROUP_WIND_POWER_BUL_COST   1000
#define GROUP_WIND_POWER_TECH   30
#define GROUP_WIND_POWER_FIREC  10
#define GROUP_WIND_POWER_RANGE  0
#define GROUP_WIND_POWER_SIZE  2

#define WIND_POWER_HIVOLT     450
#define WIND_POWER_LABOR       15
#define MAX_LABOR_AT_WIND_POWER 20*(WIND_POWER_LABOR)
#define MAX_HIVOLT_AT_WIND_POWER 20*(WIND_POWER_HIVOLT)
/* WIND_POWER_RCOST is days per quid */
#define WIND_POWER_RCOST      2
#define WIND_POWER_ANIM_SPEED 120

#define WIND_POWER_TECH 450000

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

class WindpowerConstructionGroup: public ConstructionGroup {
public:
    WindpowerConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_WIND_POWER;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_AT_WIND_POWER;
        commodityRuleCount[STUFF_HIVOLT].take = false;
        commodityRuleCount[STUFF_HIVOLT].give = true;
    }
    // overriding method that creates a Windpower
    virtual Construction *createConstruction(int x, int y);
};

extern WindpowerConstructionGroup windpowerConstructionGroup;
extern WindpowerConstructionGroup windpower_RG_ConstructionGroup;
extern WindpowerConstructionGroup windpower_G_ConstructionGroup;

class Windpower: public RegisteredConstruction<Windpower> { // Windpower inherits from its own RegisteredConstruction
public:
    Windpower(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Windpower>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        // this->anim = 0;
        this->animate_enable = false;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        // this->hivolt_output = (int)(WIND_POWER_HIVOLT + (((double)tech_level * WIND_POWER_HIVOLT) / MAX_TECH_LEVEL));
        setMemberSaved(&this->hivolt_output, "mwh_output"); // compatibility
        initialize_commodities();

        commodityMaxCons[STUFF_LABOR] = 100 * WIND_POWER_LABOR;
        // commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
    }

    virtual void initialize() override {
        RegisteredConstruction::initialize();

        this->hivolt_output = (int)(WIND_POWER_HIVOLT +
          (((double)tech * WIND_POWER_HIVOLT) / MAX_TECH_LEVEL));

        commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
    }

    virtual ~Windpower() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  hivolt_output;
    int  tech;
    int  anim;
    int  working_days, busy;
    bool animate_enable;
};

/** @file lincity/modules/windmill.h */

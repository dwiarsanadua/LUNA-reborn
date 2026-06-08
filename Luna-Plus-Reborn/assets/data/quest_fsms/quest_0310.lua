--[[
  Quest 310: [C Class] Orc Warrior Elimination
  Level Required: 41
  NPC Start: 80, NPC Complete: 80
  Prerequisites: [309]
  Rewards: EXP=137745, Gold=4000
]]

local fsm = require('fsm_engine')

local quest_310 = fsm:new({
    id = 310,
    name = "[C Class] Orc Warrior Elimination",
    level_required = 41,
    npc_start = 80,
    npc_complete = 80,
    prerequisites = {309},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 310: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 310: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 310: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 310: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 310: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 310: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 80,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 310: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 310: Rewards given")
                self:give_rewards({
                    exp = 137745,
                    gold = 4000,
                    items = {
                        { item_id = 215, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_310
--[[
  Quest 319: [B Class] Basilisk Elimination
  Level Required: 46
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [318]
  Rewards: EXP=212963
]]

local fsm = require('fsm_engine')

local quest_319 = fsm:new({
    id = 319,
    name = "[B Class] Basilisk Elimination",
    level_required = 46,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {318},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 319: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 319: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 319: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 319: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 319: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 319: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 319: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 319: Rewards given")
                self:give_rewards({
                    exp = 212963,
                    items = {
                        { item_id = 225, count = 1 },
                        { item_id = 225, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_319
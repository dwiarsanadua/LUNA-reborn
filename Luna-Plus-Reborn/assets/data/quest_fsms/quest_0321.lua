--[[
  Quest 321: [B Class] Red Feather Harpy Elimination
  Level Required: 47
  NPC Start: 81, NPC Complete: 81
  Prerequisites: [319]
  Rewards: EXP=230763
]]

local fsm = require('fsm_engine')

local quest_321 = fsm:new({
    id = 321,
    name = "[B Class] Red Feather Harpy Elimination",
    level_required = 47,
    npc_start = 81,
    npc_complete = 81,
    prerequisites = {319},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 321: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 321: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 321: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 77,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 321: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 321: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 321: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 81,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 321: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 321: Rewards given")
                self:give_rewards({
                    exp = 230763,
                    items = {
                        { item_id = 227, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_321
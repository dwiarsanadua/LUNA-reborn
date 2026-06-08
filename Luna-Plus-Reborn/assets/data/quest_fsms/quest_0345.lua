--[[
  Quest 345: [B Class] Skeleton Warrior Elimination
  Level Required: 55
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [343]
  Rewards: EXP=451316
]]

local fsm = require('fsm_engine')

local quest_345 = fsm:new({
    id = 345,
    name = "[B Class] Skeleton Warrior Elimination",
    level_required = 55,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {343},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 345: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 345: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 345: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 345: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 345: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 345: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 345: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 345: Rewards given")
                self:give_rewards({
                    exp = 451316,
                    items = {
                        { item_id = 251, count = 1 },
                        { item_id = 251, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_345
--[[
  Quest 346: [B Class] Skeleton Warrior Elimination
  Level Required: 55
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [345]
  Rewards: EXP=451316
]]

local fsm = require('fsm_engine')

local quest_346 = fsm:new({
    id = 346,
    name = "[B Class] Skeleton Warrior Elimination",
    level_required = 55,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {345},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 346: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 346: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 346: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 61,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 346: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 346: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 346: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 346: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 346: Rewards given")
                self:give_rewards({
                    exp = 451316,
                    items = {
                        { item_id = 252, count = 1 },
                        { item_id = 252, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_346
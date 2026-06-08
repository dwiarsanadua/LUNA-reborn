--[[
  Quest 341: [B Class] Skeleton Elimination
  Level Required: 51
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [327]
  Rewards: EXP=317002
]]

local fsm = require('fsm_engine')

local quest_341 = fsm:new({
    id = 341,
    name = "[B Class] Skeleton Elimination",
    level_required = 51,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {327},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 341: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 341: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 341: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 341: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 341: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 341: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 341: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 341: Rewards given")
                self:give_rewards({
                    exp = 317002,
                    items = {
                        { item_id = 247, count = 1 },
                        { item_id = 247, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_341
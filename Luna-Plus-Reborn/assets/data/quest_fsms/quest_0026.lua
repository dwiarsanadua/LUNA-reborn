--[[
  Quest 26: Breaking the Seal
  Level Required: 1
  NPC Start: 27, NPC Complete: 27
  Rewards: EXP=13660, Gold=2705
]]

local fsm = require('fsm_engine')

local quest_26 = fsm:new({
    id = 26,
    name = "Breaking the Seal",
    level_required = 1,
    npc_start = 27,
    npc_complete = 27,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 26: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 26: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 26: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 15,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 26: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 14,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 26: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 26: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 26: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 26: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 26: Rewards given")
                self:give_rewards({
                    exp = 13660,
                    gold = 2705,
                    items = {
                        { item_id = 9, count = 8 },
                        { item_id = 10, count = 10 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_26
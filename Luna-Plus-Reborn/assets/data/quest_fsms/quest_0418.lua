--[[
  Quest 418: A Bear Leather Cover
  Level Required: 19
  NPC Start: 16, NPC Complete: 16
  Rewards: EXP=7646, Gold=2000
]]

local fsm = require('fsm_engine')

local quest_418 = fsm:new({
    id = 418,
    name = "A Bear Leather Cover",
    level_required = 19,
    npc_start = 16,
    npc_complete = 16,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 418: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 418: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 418: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 67,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 418: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 418: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 418: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 418: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 418: Rewards given")
                self:give_rewards({
                    exp = 7646,
                    gold = 2000,
                    items = {
                        { item_id = 305, count = 10 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_418
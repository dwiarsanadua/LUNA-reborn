--[[
  Quest 433: [Quest] Collector
  Level Required: 50
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=304414, Gold=53235
]]

local fsm = require('fsm_engine')

local quest_433 = fsm:new({
    id = 433,
    name = "[Quest] Collector",
    level_required = 50,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 433: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 433: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 433: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 92,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 433: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 433: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 433: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 433: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 433: Rewards given")
                self:give_rewards({
                    exp = 304414,
                    gold = 53235,
                    items = {
                        { item_id = 313, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_433
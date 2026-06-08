--[[
  Quest 796: You just lost!
  Level Required: 64
  NPC Start: 91, NPC Complete: 91
  Rewards: EXP=680483, Gold=14506
]]

local fsm = require('fsm_engine')

local quest_796 = fsm:new({
    id = 796,
    name = "You just lost!",
    level_required = 64,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 796: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 796: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 796: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 256,
                    count = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 796: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 796: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 796: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 796: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 796: Rewards given")
                self:give_rewards({
                    exp = 680483,
                    gold = 14506,
                })
            end,
            transitions = {},
        },
    },
})

return quest_796
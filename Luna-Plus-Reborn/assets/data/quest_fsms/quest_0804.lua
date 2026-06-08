--[[
  Quest 804: Trouble
  Level Required: 69
  NPC Start: 39, NPC Complete: 39
  Rewards: EXP=1016762, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_804 = fsm:new({
    id = 804,
    name = "Trouble",
    level_required = 69,
    npc_start = 39,
    npc_complete = 39,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 804: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 804: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 804: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 28,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 804: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 804: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 804: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 39,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 804: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 804: Rewards given")
                self:give_rewards({
                    exp = 1016762,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_804
--[[
  Quest 403: [Quest] Army of Darkness
  Level Required: 62
  NPC Start: 90, NPC Complete: 90
  Prerequisites: [402]
  Rewards: EXP=601955, Gold=37000
]]

local fsm = require('fsm_engine')

local quest_403 = fsm:new({
    id = 403,
    name = "[Quest] Army of Darkness",
    level_required = 62,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {402},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 403: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 403: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 403: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 253,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 403: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 78,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 403: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 403: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 403: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 403: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 403: Rewards given")
                self:give_rewards({
                    exp = 601955,
                    gold = 37000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_403
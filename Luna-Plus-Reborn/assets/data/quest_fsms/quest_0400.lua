--[[
  Quest 400: [Quest] Rotting carcasses
  Level Required: 58
  NPC Start: 90, NPC Complete: 90
  Prerequisites: [399]
  Rewards: EXP=436421, Gold=16000
]]

local fsm = require('fsm_engine')

local quest_400 = fsm:new({
    id = 400,
    name = "[Quest] Rotting carcasses",
    level_required = 58,
    npc_start = 90,
    npc_complete = 90,
    prerequisites = {399},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 400: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 400: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 400: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 99,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 400: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 400: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 400: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 400: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 400: Rewards given")
                self:give_rewards({
                    exp = 436421,
                    gold = 16000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_400
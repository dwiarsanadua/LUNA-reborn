--[[
  Quest 381: [A Class] U B Trollin'
  Level Required: 65
  NPC Start: 95, NPC Complete: 95
  Prerequisites: [378]
  Rewards: EXP=781944
]]

local fsm = require('fsm_engine')

local quest_381 = fsm:new({
    id = 381,
    name = "[A Class] U B Trollin'",
    level_required = 65,
    npc_start = 95,
    npc_complete = 95,
    prerequisites = {378},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 381: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 381: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 381: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 102,
                    count = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 381: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 103,
                    count = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 381: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 381: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 381: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 381: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 381: Rewards given")
                self:give_rewards({
                    exp = 781944,
                })
            end,
            transitions = {},
        },
    },
})

return quest_381
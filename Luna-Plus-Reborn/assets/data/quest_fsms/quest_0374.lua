--[[
  Quest 374: [A Class] Shadow Hunter
  Level Required: 65
  NPC Start: 94, NPC Complete: 94
  Prerequisites: [372]
  Rewards: EXP=781944
]]

local fsm = require('fsm_engine')

local quest_374 = fsm:new({
    id = 374,
    name = "[A Class] Shadow Hunter",
    level_required = 65,
    npc_start = 94,
    npc_complete = 94,
    prerequisites = {372},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 374: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 374: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 374: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 16,
                    count = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 374: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 63,
                    count = 35,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 374: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 374: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 374: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 94,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 374: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 374: Rewards given")
                self:give_rewards({
                    exp = 781944,
                    items = {
                        { item_id = 30000503, count = 1 },
                        { item_id = 30000307, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_374
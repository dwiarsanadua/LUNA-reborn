--[[
  Quest 787: [Party] King of the Gryphon
  Level Required: 49
  NPC Start: 62, NPC Complete: 62
  Rewards: EXP=193736, Gold=7415
]]

local fsm = require('fsm_engine')

local quest_787 = fsm:new({
    id = 787,
    name = "[Party] King of the Gryphon",
    level_required = 49,
    npc_start = 62,
    npc_complete = 62,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 787: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 787: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 787: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 148,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 787: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 787: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 787: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 62,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 787: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 787: Rewards given")
                self:give_rewards({
                    exp = 193736,
                    gold = 7415,
                    items = {
                        { item_id = 21000513, count = 5 },
                        { item_id = 21000514, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_787
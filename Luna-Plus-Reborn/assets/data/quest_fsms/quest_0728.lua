--[[
  Quest 728: [Quest] Fragments of Memories
  Level Required: 50
  NPC Start: 9, NPC Complete: 404
  Rewards: EXP=317098
]]

local fsm = require('fsm_engine')

local quest_728 = fsm:new({
    id = 728,
    name = "[Quest] Fragments of Memories",
    level_required = 50,
    npc_start = 9,
    npc_complete = 404,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 728: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 404,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 728: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 728: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 59,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 728: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 404,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 728: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 728: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 728: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 728: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 404,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 728: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 728: Rewards given")
                self:give_rewards({
                    exp = 317098,
                })
            end,
            transitions = {},
        },
    },
})

return quest_728
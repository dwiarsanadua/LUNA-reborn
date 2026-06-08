--[[
  Quest 705: [Party] Defeat the Bone Cavalier
  Level Required: 0
  NPC Start: 40, NPC Complete: 40
  Prerequisites: [702]
  Rewards: EXP=216664
]]

local fsm = require('fsm_engine')

local quest_705 = fsm:new({
    id = 705,
    name = "[Party] Defeat the Bone Cavalier",
    level_required = 0,
    npc_start = 40,
    npc_complete = 40,
    prerequisites = {702},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 705: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 705: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 705: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 394,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 705: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 705: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 705: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 40,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 705: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 705: Rewards given")
                self:give_rewards({
                    exp = 216664,
                })
            end,
            transitions = {},
        },
    },
})

return quest_705